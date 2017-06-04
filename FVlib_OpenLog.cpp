#include "FVlib_OpenLog.h"

// The delay we give to the OpenLog to process the buffer
  #define dOL 100 // Less than 30 will give problems 30ok


// Constructor:
openLog::openLog(HardwareSerial &_port ) {
  hardPort = &_port;
  (*hardPort).begin(9600);

}

/*openLog::openLog(SoftwareSerial &_port) {
  hspSel = false;
  softPort = &_port;
}*/

/*byte openLog::appendToLastLoggingSession(String textToAppend) {

}*/

byte openLog::findLastLoggingSession(String loggingFileName) {
  byte index;
  long loggingFileSize;

  // Check file from 9 to 1
  for (index = 9; index != 0; index--) {
    olCommand = String(loggingFileName + String(index) + ".txt");
    //Serial.println(olCommand);
    loggingFileSize = fileSize(olCommand);
    Serial.print(olCommand);
    Serial.println(loggingFileSize);
    if ((loggingFileSize != 0) && (index == 9)) return -1;
    if (loggingFileSize != 0) return index;
  }
  return 1;
}

long openLog::fileSize(String fileName) {
  long loggingFileSize;
  char recivedChar;

  // Clear the buffer:
  (*hardPort).println("");
  delay(dOL);
  while((*hardPort).available() > 0) (*hardPort).read();

  // Find the size:
  olCommand = String("size " + fileName);
  (*hardPort).println(olCommand);
  delay(dOL);
  loggingFileSize = 0;
  recivedChar = (*hardPort).read();
  if (recivedChar == '\r') {
    recivedChar = (*hardPort).read();
    if (recivedChar == '\n') {
      while((*hardPort).available() > 0) {
        recivedChar = (*hardPort).read();
        if ((recivedChar == '!') || (recivedChar == '-')) {
          while((*hardPort).available() > 0) (*hardPort).read();
          break;
        }
        else if ((recivedChar >= '0') && (recivedChar <= '9')) {
          loggingFileSize = loggingFileSize + (recivedChar - '0');
          if (loggingFileSize < 1000000000) loggingFileSize = loggingFileSize * 10;
        }
      }
      if (recivedChar == '-') { // "-1"
        // File does not exist:
        //Serial.println("File does not exist!");
        return 0;
      }
      else if (recivedChar == '!') {
        // Error:
        //Serial.println("Error");
        return 0;
      }
      else {
        if (loggingFileSize < 1000000000) loggingFileSize = loggingFileSize / 10;
        //Serial.println(loggingFileSize);
        return loggingFileSize;
      }
    }
  }
  else {
    // Error:
    return 0;
  }
}
