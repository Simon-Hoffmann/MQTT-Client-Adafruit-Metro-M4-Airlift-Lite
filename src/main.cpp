#include "wifi.h"

void setup() {
  wifisetup();
  
}

void loop() {
  // check the network connection once every 10 seconds:
  delay(10000);
  printCurrentNet();
  
}
