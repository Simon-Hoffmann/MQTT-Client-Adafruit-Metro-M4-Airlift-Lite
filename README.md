# MQTT Client - Adafruit Metro M4 Express Airlift Lite

![Adafruit IO Dashboard](src/img/AdafruitIO.png?raw=true "Title")

## Functionality

A MQTT Client which:
  - Publishes and subscribes to feeds
  - Has a temperature, humidity and motion sensor
  - Can toggle a sensor remotely through subscribed feed (Motion sensor)
  
### Publishing

- Microcontroller checks time over internet to send temperature and humidity data to Adafruit IO every 6 hours.
- Motion data is sent if the sensor has been remotely activated.

### Subscribing

- A constant message query is active to receive subscribed messages, this is used to activate/deactivate the motion sensor.

## Setup

![Adafruit Metro M4 Express Airlift Lite](src/img/1620491395507.jpg?raw=true "Title")

## Libraries Used

- Adafruit_FONA_Library
- Adafruit_IO_Arduino
- Adafruit_MQTT_Library
- Adafruit_SleepyDog_Library
- Adafruit_Unified_Sensor
- DHT_sensor_library
- WiFiNINA-master
