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

/****************************** Funcs ***************************************/

void mqttSetup(){
    //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect.
  }

  // check for the wifi module
  while (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    delay(1000);
  }

  #ifdef DEBUG
    String fv = WiFi.firmwareVersion();
    if (fv < "1.0.0")
    {
      Serial.println("Please upgrade the firmware");
    }
  #endif

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(WLAN_SSID);
  do
  {
    status = WiFi.begin(WLAN_SSID, WLAN_PASS);
    delay(100); // wait until connected
  } while (status != WL_CONNECTED);
  Serial.println("Connected to wifi");

  
  #ifdef DEBUG
    printWiFiStatus();
  #endif
}

void publishMessage(char* t, uint32_t h){
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  
  temperature.publish(t);
  delay(2000);
  humidity.publish(h);


  // wait a couple seconds to avoid rate limit
  delay(2000);
}

void publishMessageMotion(){
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  #ifdef DEBUG
    if (!motionTime.publish(""))
    {
      Serial.println(F("Motion sending failed"));
    }
    else
    {
      Serial.println(F("Motion sent"));
    }
  #endif
  
  #ifndef DEBUG
    motionTime.publish("");
  #endif

  // wait a couple seconds to avoid rate limit
  delay(2000);
}


// Function to connect and reconnect as necessary to the MQTT server.
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

  Serial.print("Connecting to MQTT... ");
  
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
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

  Serial.println("MQTT Connected!");
  lastwill.publish("online");
}

#ifdef DEBUG
void printWiFiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
#endif
