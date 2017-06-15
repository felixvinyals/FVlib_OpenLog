#include "FVlib_OpenLog.h"

openLog sd(Serial2, 8, true); // [Serial port connected to the OpenLog], [Which pin of the Arduino is connected the Vcc of OpenLog], [verbose mode] 
byte sdAppendAsnwer;

void setup() {
  Serial.begin(9600);

}

void loop() {

  Serial.println("------------------------------");
  String data = String(millis());

  sdAppendAsnwer = sd.appendToLastLoggingSession("CSO", data, 350); // [File prefix], [data to be logged], [file size limit in Mb]
  if (sdAppendAsnwer == 0) {
    Serial.println(String("Data " + data + " logged to SD!"));
  }
  else if (sdAppendAsnwer == 1) {
    Serial.println("Error!");
  }
  else if (sdAppendAsnwer == 2) {
    Serial.println("No free loggig sessions");
  }
  //delay(100);
  
}