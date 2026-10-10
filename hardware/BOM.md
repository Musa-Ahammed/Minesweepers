# Bill of materials

[Back to the project overview](../README.md)

Confirm component models, voltage ratings, and wiring against the assembled robot.

| Quantity | Part | Notes |
| ---: | --- | --- |
| 1 | Arduino Mega 2560 | Controller used by the sketch and hardware images. |
| 1 set | FlySky FS-CT6B transmitter and receiver | Throttle and steering use two receiver channels. |
| 4 | DC gear motor | Two motors per side. |
| 1 | Dual-channel Cytron brushed motor driver | Must suit the battery voltage and combined stall current per side. |
| 1 | 3S 11.1 V LiPo battery | 12.6 V when fully charged. |
| 1 | 5 V buck converter | Regulated logic supply. |
| 3 | HC-SR04-type ultrasonic sensor | Front, left, and right distance readings. |
| 1 set | Inductive coil and detector circuit | Analog output to A0; circuit details are not included. |
| 1 | 16×2 LCD with I2C backpack | Sketch uses address `0x27`. |
| 1 | HC-05 Bluetooth module | Telemetry through `Serial1` at 9600 baud. |
| 1 set | LED and resistor | Detection indicator. |
| 1 | Buzzer | Confirm compatibility with the sketch's HIGH/LOW output and pin current limits. |
| 1 | Main switch / emergency disconnect | Physical power isolation. |
| 1 | Main fuse and holder | Size for the wiring and expected load. |
| 1 | Carrier/interface PCB | Reference images included; current capacity is unverified. |

See the [schematic](Sch-MS.jpg), [PCB layout](PCB-MS.jpg), and [firmware pin table](../README.md#firmware-connections). The image and sketch connections need to be matched before use.
