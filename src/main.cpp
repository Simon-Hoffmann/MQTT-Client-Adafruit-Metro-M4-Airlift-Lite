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
 */

 //To add:
 // Interrupt for Motion sensor, takes to long for it to read sensor, since message query needs most time (just change variable in interrupt) Activate and deactivate interupt according to received motion status

#include "Mqtt.h"
#include "DHT.h"

/**************************** Temp and Humidity Sensor Setup***********************************/
#define DHTPIN 2  
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

/***************************** Motion Detector Sensor Setup************************************/

int inputPin = 3;               // PIR sensor pin
int pirState = LOW;             
int val = 0;                    // variable for reading the pin status

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

  //Activates Motion sensor if activated from Adafruit IO 
  if(motionOn){
    val = digitalRead(inputPin);  // read input value
    if (val == HIGH) {            // check if the input is HIGH
      if (pirState == LOW) {
        // publish to Adafruit IO when motion detected
        publishMessageMotion("Adafruit says: Motion Detected!!");
        //only output change, not state
        pirState = HIGH;
      }
    } else {
      if (pirState == HIGH){
      pirState = LOW;
      }
    }
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
