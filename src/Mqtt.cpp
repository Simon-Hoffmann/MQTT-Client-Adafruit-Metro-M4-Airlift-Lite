#include "Mqtt.h"
#include "Wifi.h"

int status = WL_IDLE_STATUS;

/**************************** Global State ***********************************/

// WiFiSSLClient for SSL/TLS support
WiFiSSLClient client;

// Setup the MQTT client class by WLAN_PASSing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish motionTime = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/motionTime");
Adafruit_MQTT_Publish lastwill = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/status", MQTT_QOS_1);

Adafruit_MQTT_Subscribe motion = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/motion");

/****************************** Funcs ***************************************/

//Is set when Interrupt activated so motion will be sent
bool motionSend = false;

//Motion Sensor
int inputPin = 3;               // PIR sensor pin         

void mqttSetup(){
  // check for the wifi module
  while (WiFi.status() == WL_NO_MODULE)
  {
    delay(1000);
  }

  // attempt to connect to Wifi network:
  do
  {
    status = WiFi.begin(WLAN_SSID, WLAN_PASS);
    delay(100); // wait until connected
  } while (status != WL_CONNECTED);

  //subscribe to motion Adafruit IO
  mqtt.subscribe(&motion);
}

/*
 * Checks for incoming messages from Adafruit IO and activates/deactivates Interrupt
 */
void messageQuery(){
  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(5000))) {
    // Check if its the motion feed
    if (subscription == &motion) {
      if (strcmp((char *)motion.lastread, "ON") == 0) {
        //Interrupt activated when motion turned on from Adafruit IO, this Interrupts when signal received from motion sensor, only when rising signal detected
        attachInterrupt(digitalPinToInterrupt(inputPin), motion_ISR, RISING);
        publishMessageMotion("Adafruit says: Motion Detection ON");
      }
      if (strcmp((char *)motion.lastread, "OFF") == 0) {
         //remove Interrupt
         detachInterrupt(digitalPinToInterrupt(inputPin));
         publishMessageMotion("Adafruit says: Motion Detection OFF");
      }
    } 
  }
    // ping the server to keep the mqtt connection alive
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

}

/*
 * Interrupt Service Routine for motion sensor, this changes a variable so motion message is sent later, doesnt have to be instantly
 */
void motion_ISR(){
  motionSend = true;
}

/*
 * For publishing Temperature and Humidity from DHT11 Sensor
 */
void publishMessage(char* t, uint32_t h){
  // Ensure the connection to the MQTT server is alive
  MQTT_connect();
  
  temperature.publish(t);
  // wait a couple seconds to avoid rate limit
  delay(2000);
  humidity.publish(h);
}

/*
 * For publishing Motion Sensor
 */
void publishMessageMotion(char* m){
  // Ensure the connection to the MQTT server is alive
  MQTT_connect();

  motionTime.publish(m);

  // wait a couple seconds to avoid rate limit
  delay(2000);
}

/*
 * Function to connect and reconnect as necessary to the MQTT server.
 */
void MQTT_connect()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }
  
  //Last Will Message Set
  mqtt.will(AIO_USERNAME "/feeds/status" , "offline", MQTT_QOS_1, 1);


  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    mqtt.disconnect();
    delay(5000); // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      // wait for reset
      while (1)
        ;
    }
  }
  lastwill.publish("online");
}
