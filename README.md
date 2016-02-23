# node-dht-sensor

This node.js module supports querying air temperature and relative humidity from a compatible DHT sensor.

The module is forked from https://github.com/momenso/node-dht-sensor.
I made the following adjustments:
- Replaced BCM2835 library with wiringPi. It still uses the Broadcom GPIO pin numbers. Thanks to @Xento.
