#include "Mqtt.h"
#include "DHT.h"

/**************************** Temp and Humidity Sensor ***********************************/
#define DHTPIN 2  
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

bool tempHumiditySent = false;

void setup()
{
  mqttSetup();
  dht.begin();
  MQTT_connect();
}

void loop()
{
  if(getHourTime()){
    uint32_t h = dht.readHumidity();
    float t = dht.readTemperature();
    //Adafruit IO Dashboard throws errors when sending floats, so it's a char[] now
    String temptemp = String(t);
    char *temp = new char[temptemp.length() + 1];
    strcpy(temp, temptemp.c_str());
    // do stuff
    publishMessage(temp, h);
    delete [] temp;
  }
  //subscribeMessage();
  delay(20000);
  //
}

//For sending Data every 6 Hours
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
