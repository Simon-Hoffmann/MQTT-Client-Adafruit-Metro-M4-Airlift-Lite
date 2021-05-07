#include <WiFiNINA.h>
#include <SPI.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define MQTT_CONN_KEEPALIVE 60

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 8883
#define AIO_USERNAME "********************"
#define AIO_KEY "********************"

/****************************** Funcs ***************************************/

extern bool motionOn;

void mqttSetup();
void messageQuery();
void subscribeMessage();
void publishMessage(char* t, uint32_t h);
void publishMessageMotion(char* m);
void MQTT_connect();
