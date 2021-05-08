/*  Adafruit Metro M4 Express Airlift Lite - MQTT Client
 *  ---------------------------------------------------
 *  
 *  MQTT Client which publishes Temperature and Humidity every 6 Hours to Adafruit IO, this is done by:
 *  
 *    -Getting time from the internet to send every 6 hours
 *    
 *    -When the time is right, Temperature and Humidity is read from the DHT11 Sensor
 *    
 *    -These values are then published over the MQTT protocol
 *  
 *  MQTT Client which subscribes to the motion feed from Adafruit IO to activate/deactivate motion sensor:
 *  
 *    -When the value of "motion" on the Adafruit IO broker is changed (e.g. a button) a message is published
 *    
 *    -This MQTT Client is subscribed to the motion feed and constantly Queries for incoming messages
 *    
 *    -When a message has been received and it is from the Motion feed this activates the reading of the motion sensor which then publishes if a motion is detected
 *    
 *    -When motion is activated, an interrupt is set for the motion sensor pin, since the program spends a lot of time in the messageQuery func
 */

#include "Mqtt.h"
#include "DHT.h"

/**************************** Temp and Humidity Sensor Setup***********************************/
#define DHTPIN 2  
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
/**********************************************************************************************/

bool tempHumiditySent = false;

//counter for loop
uint16_t count = 0;

void setup()
{
  mqttSetup();
  dht.begin();  
  pinMode(inputPin, INPUT);     // declare motion sensor as input
}

void loop()
{
  count++;
  MQTT_connect();

  //Checks for incoming Messages from Adafruit IO
  messageQuery();

  //Checks If Interrupts for motion Sensor changed variable to send data
  if(motionSend){
    publishMessageMotion("Adafruit says: Motion Detected!!");
    motionSend = false;
  }

  //Check time, doesn't have to happen a lot since only hour is relevant
  if(count > 100){
    count = 0;
    
    if(getHourTime()){
        //Get readings from DHT11 Sensor
        uint32_t h = dht.readHumidity();
        float t = dht.readTemperature();
        
        //Adafruit IO Dashboard throws errors when sending floats, so it's a char[] now
        //MQTT Library publish doesn't support Strings
        String temptemp = String(t);
        char *temp = new char[temptemp.length() + 1];
        strcpy(temp, temptemp.c_str());
        
        //Publish temp and humidity readings to Adafruit IO 
        publishMessage(temp, h);
        delete [] temp;
    }
  }
}

/*
 * For sending Data every 6 Hours, checks time of day (Hour)
 */
bool getHourTime(){
  
  //getTime is Seconds since 1970, calc to Hour today + timezone
  int Hour = (WiFi.getTime() % 86400) / 60 / 60 + 2;
  
  //check if it is the right time and if Data has already been sent in the same Hour or not
  if((Hour % 6 == 0) && (!tempHumiditySent)){
    tempHumiditySent = true;
    return true;
  } else if ((Hour % 6 == 0) && (tempHumiditySent)){
    return false;
  } else{
    tempHumiditySent = false;
    return false;
  }
}
