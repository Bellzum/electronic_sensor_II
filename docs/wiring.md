# Wiring (Arduino Uno)

## Parts
Arduino Uno · RC522 module + cards/tags · 16x2 LCD with I²C backpack · SG90 servo · breadboard · jumper wires (M-M and F-M) · USB cable

## RC522 RFID (SPI) — 3.3 V!
| RC522 | Uno |
|---|---|
| SDA (SS) | D10 |
| SCK | D13 |
| MOSI | D11 |
| MISO | D12 |
| IRQ | not connected |
| GND | GND |
| RST | D9 |
| 3.3V | 3.3V |

## LCD I²C
| LCD | Uno |
|---|---|
| GND | GND |
| VCC | 5V |
| SDA | A4 |
| SCL | A5 |

## SG90 servo
| Wire | Uno |
|---|---|
| Brown | GND |
| Red | 5V |
| Orange (signal) | D3 |

## Sensor review cheat-sheet
| Part | Signal type | Simple meaning |
|---|---|---|
| RC522 | SPI digital | Fast 4-wire conversation, Arduino is the boss |
| LCD | I²C digital | 2-wire shared bus, each device has an address |
| Servo | PWM output | Pulse width = angle |
