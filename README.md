# nixie-tube-clock


## Hardware

### Measurements
  * Cathode voltage: 178 V
  * Cathode current: ~800 uA
  * Glim lamp current: ~250 uA
  * Total current: ~220 mA @ 12,2 V without WiFi.
  
### Known issues
  * RBG LED flahes at power-on. The gate of MOSFET of LEDs require a pull-down resistor.
  * PCB design of tube is wrong. The place drawing of tube is mirrored; therefore, digit 7 is digit 4 in real.
