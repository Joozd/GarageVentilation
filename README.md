# GarageVentilation

**Battery-powered, humidity-aware garage ventilation using an ESP32-C5, Bluetooth sensors and a 5 V fan.**

GarageVentilation is a small DIY project that decides **when ventilating a garage will actually help remove moisture**. Instead of switching a fan on whenever the relative humidity is high, it compares the **absolute humidity** of the air inside and outside. The fan should run only when the garage is humid enough *and* the outside air contains sufficiently less water vapor.

The project combines an ESP32-C5, two existing Bluetooth sensors, a MOSFET-switched fan, and a rechargeable battery. It is being developed with PlatformIO and includes a custom PCB designed in KiCad.

> **Status: work in progress.** BLE measurement decoding, ventilation decision logic, fan switching and sleep behavior are being developed and tested. This is a prototype, not a finished commercial device.

## How it works

```text
    SwitchBot (inside) ── BLE ──┐
                               │
                               ▼
                         ESP32-C5
                               ▲
                               │
    RuuviTag (outside) ── BLE ─┘
                               │
                     Compare humidity
                               │
                   Ventilation worthwhile?
                         /           \
                       yes            no
                        │              │
                   MOSFET → fan     Deep sleep
                        │
                    Light sleep
                        │
                  Measure again
```

1. Scan Bluetooth advertisements from the **indoor SwitchBot** and **outdoor RuuviTag**.
2. Decode temperature and relative humidity from both sensors.
3. Calculate absolute humidity in g/m³.
4. Start the fan if the indoor relative humidity is high enough and the indoor air contains sufficiently more moisture than the outside air.
5. Use different start/continue thresholds (*hysteresis*) to avoid rapidly switching the fan on and off.
6. Sleep between measurements to conserve battery power.

If a measurement cannot be obtained, the controller takes a conservative approach instead of assuming ventilation is useful. While ventilating, the current firmware allows one missed measurement cycle before stopping the fan.

### Why absolute humidity?

Relative humidity alone is misleading when temperatures differ. Cold outside air can have a high relative humidity while containing less water vapor than warmer garage air. Once that outside air warms up in the garage, its relative humidity falls.

Comparing **absolute humidity** gives a better indication of whether exchanging the air will remove moisture. It does not guarantee that every surface will become drier immediately: surface temperature, condensation, airflow and moisture sources still matter.

### Current control thresholds

These are **experimental firmware settings**, not universal recommendations:

| Condition | Start ventilating | Continue ventilating |
| --- | ---: | ---: |
| Minimum indoor relative humidity | 66% | 64% |
| Minimum indoor–outdoor absolute-humidity difference | 2.0 g/m³ | 1.5 g/m³ |

Both conditions must be satisfied. The lower continuation thresholds provide hysteresis.

## Hardware

| Component | Purpose |
| --- | --- |
| Seeed Studio XIAO ESP32-C5 | Microcontroller and BLE receiver |
| SwitchBot temperature/humidity sensor | Indoor measurement via BLE advertisements |
| RuuviTag | Outdoor temperature/humidity measurement via BLE RAWv2 |
| 5 V Noctua fan | Exhaust air from the garage |
| Logic-level N-channel MOSFET | Switch the fan from an ESP32 GPIO |
| Seeed LiPo Rider Plus | Rechargeable battery power and 5 V boost output |
| Single-cell Li-ion battery pack | Portable power source |
| Custom KiCad PCB | Mount and connect the electronics |

### Fan wiring (simplified)

```text
5 V boost output ───────── Fan +
                           Fan − ─── MOSFET drain
ESP32 GPIO (D2) ────────── MOSFET gate
Ground ─────────────────── MOSFET source
```

The ESP32 and fan power supply **must share a common ground**. A suitable gate resistor and gate pull-down are part of the MOSFET control circuit. The fan is switched on the low side; it is **not** powered directly by an ESP32 GPIO.

The ESP32 is powered from the boost converter's 5 V output. Consult the XIAO ESP32-C5 and LiPo Rider Plus documentation before reproducing the power connections, particularly when combining external power and USB.

> **Battery safety:** Use a protected battery pack and a charging/power board appropriate for its chemistry. Check polarity, current limits and mechanical protection before operating unattended.

## Firmware architecture

The code is organized so that measurement collection, humidity decisions, hardware switching and sleep management have separate responsibilities.

```text
main.cpp
  ├── MeasurementsScanner
  │     ├── BleScanner (NimBLE + FreeRTOS queue)
  │     ├── AdvertisementClassifier
  │     ├── RuuviDecoder
  │     └── SwitchbotDecoder
  ├── VentilationController
  ├── Fan
  └── sleep utilities
```

- **`MeasurementsScanner`** gathers and decodes the two sensor measurements. It returns either a complete pair or no measurement.
- **`BleScanner`** handles passive BLE scanning. Incoming advertisements are copied into a bounded FreeRTOS queue and processed by the main application task. Bluetooth is initialized at the start of each scan and deinitialized afterward, so it does not remain active during light sleep.
- **`VentilationController`** decides whether ventilation is worthwhile using measurement data and configurable thresholds; it does not manipulate hardware.
- **`Fan`** owns the output GPIO and controls the MOSFET, including GPIO hold so the output stays high during light sleep.
- **Sleep utilities** put the controller into light sleep while ventilating or deep sleep when ventilation is not needed.

Deep sleep restarts the program on wake-up. Light sleep resumes execution where the program left off.

## Building and flashing

The firmware uses **C++**, the **Arduino framework**, **PlatformIO** and **NimBLE-Arduino**.

1. Install [Visual Studio Code](https://code.visualstudio.com/) and the [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode).
2. Open this repository as a folder in VS Code.
3. Connect the XIAO ESP32-C5 over USB.
4. In PlatformIO, select the `seeed-xiao-esp32-c5` environment and build/upload the firmware.
5. Open the serial monitor at **115200 baud** for diagnostic output.

The project's `platformio.ini` defines the board, framework and library dependencies. Use the configuration committed with the code rather than copying settings from an older version of this README.

## Power-saving approach

The prototype aims for a low average current draw rather than continuously operating the radio and fan:

- **Fan off:** stop scanning, shut down Bluetooth and enter deep sleep until the next measurement cycle.
- **Fan on:** keep the MOSFET gate asserted, shut down Bluetooth and use light sleep between measurement cycles.
- **After waking:** initialize Bluetooth again for the next scan.

A fan duty cycle around **10%** is an initial design target, not a measured long-term result. Actual battery life depends on fan runtime, battery capacity, boost-converter losses, measurement frequency and standby current. No runtime guarantee is made yet.

## PCB and mechanical design

A custom PCB is designed in **KiCad**. The intent is to keep the electronics easy to assemble and allow the battery to sit in a separate, mechanically secured holder. The LiPo Rider Plus charging connection should remain accessible.

The repository can contain the PCB sources alongside the firmware, for example:

```text
GarageVentilation/
├── src/
├── include/
├── lib/
├── hardware/
│   └── pcb/
│       ├── *.kicad_pro
│       ├── *.kicad_sch
│       └── *.kicad_pcb
├── platformio.ini
└── README.md
```

This layout is illustrative; the actual folder structure may evolve.

## Project status / next steps

- [x] Decode RuuviTag BLE RAWv2 measurements
- [x] Decode SwitchBot temperature/humidity advertisements
- [x] Compare inside/outside absolute humidity
- [x] Drive the fan through a MOSFET
- [x] Test GPIO hold across light sleep
- [x] Design an initial PCB in KiCad
- [ ] Verify reliable repeated BLE scans across sleep cycles
- [ ] Validate long-term power consumption and battery runtime
- [ ] Finish mechanical mounting and battery holder
- [ ] Test control thresholds against real garage conditions
- [ ] Document the finished wiring and add build photos

## A note on scope

This is a purpose-built garage ventilator, not a general home automation platform. The design deliberately favors a small, readable firmware structure and a simple hardware interface over unnecessary abstraction.

---

Built as a practical electronics, embedded C++ and humidity-control learning project.
