# node-dht-sensor

This node.js module supports querying air temperature and relative humidity from a compatible DHT sensor.

The module is forked from https://github.com/momenso/node-dht-sensor.
I made the following adjustments:
- Replaced BCM2835 library with wiringPi. It still uses the Broadcom GPIO pin numbers. Thanks to @Xento.

## Installation
``` bash
$ npm install git+https://github.com/HardwareProjects/node-dht-sensor.git
```

See http://wiringpi.com/wiringpi-update-to-2-29/ for using it without being roo or using sudo.
