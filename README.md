# nixie-tube-clock

![ezgif com-gif-maker-3](https://user-images.githubusercontent.com/8822138/207956721-9b5c9171-c8a4-4f95-b3d6-a76d2fb03cdc.gif)

![IMG_2297](https://user-images.githubusercontent.com/8822138/207957011-6f1c9aab-68bf-4625-8830-9e2ad03c3db7.jpg)

![IMG_2301](https://user-images.githubusercontent.com/8822138/207957017-dbd5976a-45c0-4ff2-803f-f5abd02fd5ed.jpg)

At first I declared my requirements for my ideal clock. Here are the requirements:
  * No touch working: there shall not be any button or something like that but the clock shall follow daylight saving time or it shall be set after a power outage.
  * Dust resistant: the dust shall not affect the visibility of clock and it shall not be on the PCB and parts.
  * Beautiful: The clock shall have maximum 2 dominant colors (PCB, parts and enclosure).
  
To implement "no touch working" I used a Wifi module to query the accureate time from my home server.
For "dust resistant" requirement I put the clock in a plastic enclosure to procect the core of clock.
"Beautiful" is implemented by selecting white PCB and mostly black parts.

## Docker

```
docker build -t time-server:v1.0 .
docker-compose up -d
```

## Hardware

### Measurements
  * Cathode voltage: 178 V
  * Cathode current: ~800 uA
  * Glim lamp current: ~250 uA
  * Total current: ~220 mA @ 12,2 V without WiFi.
  
### Known issues
  * RBG LED flahes at power-on. The gate of MOSFET of LEDs require a pull-down resistor.
  * PCB design of tube is wrong. The place drawing of tube is mirrored; therefore, digit 7 is digit 4 in real.
