# MQTT Client - Adafruit Metro M4 Airlift Lite

//IO Picture

## Functionality

A MQTT Client which:
  -Publishes and subscribes
  -Has a temperature, humidtity and motion sensor
  -Can toggle a sensor remotely through subscribed feed
  
### Publishing

-Microcontroller checks time over internet to send temperature and humidity data to Adafruit IO every 6 hours.
-Motion data is sent when subscribed data is received from a specific feed to activate motion sensor interrupt.

### Subscribing

-A constant message query is active to receive subscribed messages, this is used to activate/deactivate the motion sensor.

## Setup

//Adafruit Image

## Dashboard

//Adafruit IO Dashboard Image
