#include "FVlib_OpenLog.h"

// Delay OpenLog, needed for the OpenLog to process the buffer
  #define dOL 100 // Less than 30 will give problems 30ok


// Constructor:
openLog::openLog(HardwareSerial &_port, unsigned int MBfileSizeLimit) {
  hardPort = &_port;
  (*hardPort).begin(9600);

}

/*openLog::openLog(SoftwareSerial &_port) {
  hspSel = false;
  softPort = &_port;
}*/

byte openLog::appendToLastLoggingSession(String loggingFileName, String textToAppend) {
  char recivedChar;

  // Initialize SD, useful if SD stopped working
  (*hardPort).println("init");
  if (!waitForChar('>')) return 10;


  lastLoggingSession = findLastLoggingSession(loggingFileName);
  if (lastLoggingSession == 255) return 1;

  olCommand = String("append " + loggingFileName + lastLoggingSession + ".txt");

  // Clear the buffer:
  (*hardPort).write(26);
  (*hardPort).write(26);
  (*hardPort).write(26);
  (*hardPort).println("");
  delay(dOL);
  while((*hardPort).available() > 0) (*hardPort).read();

  // Enter append mode:
  (*hardPort).println(olCommand);
  delay(dOL);
  if ((char((*hardPort).read()) == '\r') && (char((*hardPort).read()) == '\n') && (char((*hardPort).read()) == '<')) {
    // We're in append mode now
    Serial.println("In append mode");
    (*hardPort).println(textToAppend);
    delay(dOL);
  }
  else { // Append mode could not be reached
    Serial.println("Error getting in append mode");
    return 1;
  }

  // Exit append mode:
  Serial.println("Exiting append mode");
  (*hardPort).write(26);
  (*hardPort).write(26);
  (*hardPort).write(26);
  delay(dOL);
  delay(dOL);
  delay(dOL);
  timeOut = millis();
  while((millis() - timeOut) < 2000) {
    if ((*hardPort).available() > 0) {
      recivedChar = (*hardPort).read();
      Serial.write(recivedChar);
      if (recivedChar == '>') {
        return 0;
      }
    }
  }
  return 2;
}

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
    if ((loggingFileSize != 0) && (index == 9)) return 255; // No available logging sessions
    if (loggingFileSize != 0) return index; // Available logging session found
  }
  return 1; // Logging session '1' is the last one
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

boolean openLog::waitForChar(char whichChar) {
  char recivedChar;
  timeOut = millis();
  while((millis() - timeOut) < 2000) {
    if ((*hardPort).available() > 0) {
      recivedChar = (*hardPort).read();
      //Serial.write(recivedChar);
      if (recivedChar == whichChar) {
        return true;
      }
    }
  }
  return false;
}
