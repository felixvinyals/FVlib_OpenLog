#include "FVlib_OpenLog.h"

// The delay we give to the OpenLog to process the buffer
  #define dOL 100 // Less than 30 will give problems 30ok


// Constructor:
openLog::openLog(HardwareSerial &_port) {
  hspSel = true;
  hardPort = &_port;
  (*hardPort).begin(9600);
}

/*openLog::openLog(SoftwareSerial &_port) {
  hspSel = false;
  softPort = &_port;
}*/


byte openLog::findLastLoggingSession(String loggingFileName) {
  byte index;

  char recivedChar;

  (*hardPort).println("");
  delay(dOL);
  while((*hardPort).available() > 0) (*hardPort).read(); // Clean the buffered

  for (index = 9; index != 0; index--) {
    olCommand = String("size " + loggingFileName + String(index) + ".txt");
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
        Serial.print("File: ");
        Serial.print(olCommand);
        Serial.print(" size: ");
        if (recivedChar == '-') {// If result is -1 file does not exist
          Serial.println("File does not exist!");
        }
        else if (recivedChar == '!') {
          Serial.println("Error");
        }
        else {
          if (loggingFileSize < 1000000000) loggingFileSize = loggingFileSize / 10;
          Serial.println(loggingFileSize);
        }
      }
    }
    else {
      Serial.print(olCommand);
      Serial.print(" Saltat!:");
    }
  }
  return 0;
}

long openLog::fileSize(String fileName) {
  long loggingFileSize;

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
      Serial.print("File: ");
      Serial.print(olCommand);
      Serial.print(" size: ");

      if (recivedChar == '-') {// If result is -1 file does not exist
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
