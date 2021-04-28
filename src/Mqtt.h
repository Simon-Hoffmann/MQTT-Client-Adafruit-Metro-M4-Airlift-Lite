#include <WiFiNINA.h>
#include <SPI.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define MQTT_CONN_KEEPALIVE 60
//#define DEBUG

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 8883
#define AIO_USERNAME "***************"
#define AIO_KEY "**************"

/****************************** Funcs ***************************************/

void mqttSetup();
void publishMessage(char* t, uint32_t h);
void publishMessageMotion();
void MQTT_connect();
#ifdef DEBUG
  void printWiFiStatus();
#endif
