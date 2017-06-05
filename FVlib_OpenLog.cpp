#include "FVlib_OpenLog.h"

// Delay OpenLog, needed for the OpenLog to process the buffer
  #define dOL 30 // Less than 30 will give problems 30ok


// Constructor:
openLog::openLog(HardwareSerial &_port, byte _openLogVCCpin) {
  openLogVCCpin = _openLogVCCpin;
  hardPort = &_port;

  (*hardPort).begin(9600);
  pinMode(openLogVCCpin, OUTPUT);
  digitalWrite(openLogVCCpin, HIGH);

}

/*openLog::openLog(SoftwareSerial &_port) {
  hspSel = false;
  softPort = &_port;
}*/

byte openLog::appendToLastLoggingSession(String loggingFileName, String textToAppend, unsigned int _MBfileSizeLimit) {
// Return 0:Ok! 1:Fail
  byte result = 1;
  for (byte attempts = 0; attempts < 3, attempts++) {
    result = doAppendToLastLoggingSession(loggingFileName, textToAppend, _MBfileSizeLimit);
    if (result == 0) return 0;
  }
  return 1; // Fail to co
}


byte openLog::doAppendToLastLoggingSession(String loggingFileName, String textToAppend, unsigned int _MBfileSizeLimit) {
// Return 0:Ok 1:Error
  char recivedChar;

  // Initialize SD, useful if SD stopped working
  (*hardPort).println("init");
  if (!waitForChar('>')) return 1;


  lastLoggingSession = findLastLoggingSession(loggingFileName, _MBfileSizeLimit);
  if (lastLoggingSession == 255) return 1;

  olCommand = String("append " + loggingFileName + lastLoggingSession + ".txt");

  // Clear the buffer:
  (*hardPort).write(26);
  (*hardPort).write(26);
  (*hardPort).write(26);
  (*hardPort).println("");
  if (!waitForChar('>')) return 1;

  // Enter append mode:
  (*hardPort).println(olCommand);
  if (waitForChar('<')) {
    // We're in append mode now
    (*hardPort).print(textToAppend);
  }
  else {
    // Append mode could not be reached
    return 1;
  }

  // Exit append mode:
  (*hardPort).write(26);
  (*hardPort).write(26);
  (*hardPort).write(26);
  if (waitForChar('>')) {
    (*hardPort).println("sync");
    if (waitForChar('>'))  return 0; // Back to the command mode, everything was done!
  }
  else return 1; // We got stuck in the command mode, error.
}

byte openLog::findLastLoggingSession(String loggingFileName, unsigned int _MBfileSizeLimit) {
// Return:
  // 255:No available logging sessions/Error
  // else:logging session number
  byte index;
  long loggingFileSize, limitFileSizeBytes;
  limitFileSizeBytes = (_MBfileSizeLimit * 1048576); //1mb = 1048576bytes

  // Check file from 9 to 1
  for (index = 9; index != 0; index--) {
    olCommand = String(loggingFileName + String(index) + ".txt");
    loggingFileSize = fileSize(olCommand);
    Serial.print(olCommand);
    Serial.println(loggingFileSize);
    if ((index == 1) && (loggingFileSize < limitFileSizeBytes)) {
      Serial.println("Cas 1");
      return index;
    }
    else if ((index == 1) && (loggingFileSize >= limitFileSizeBytes)) {
      Serial.println("Cas 2");
      return ++index;
    }
    else if ((index == 9) && (loggingFileSize >= limitFileSizeBytes)) {
      Serial.println("Cas 3");
      return 255;
    }
    else if ((loggingFileSize > 0) && (loggingFileSize < limitFileSizeBytes)) {
      Serial.println("Cas 4");
      return index;
    }
    else if (loggingFileSize >= limitFileSizeBytes) {
      Serial.println("Cas 5");
      return ++index;
    }
    else return 255;
  }
}

long openLog::fileSize(String fileName) {
//Return: 0:Error or no file !=0:File Size
  long loggingFileSize;
  char recivedChar;

  // Clear the buffer:
  (*hardPort).println("");
  if (!waitForChar('>')) return 0;

  // Find the size:
  olCommand = String("size " + fileName);
  (*hardPort).println(olCommand);
  delay(dOL);
  loggingFileSize = 0;

  if (waitForChar('\n') == true) {
    while((*hardPort).available() > 0) {
      recivedChar = (*hardPort).read();
      if ((recivedChar == '!') || (recivedChar == '-')) {
        while((*hardPort).available() > 0) (*hardPort).read();
        return 0;
      }
      else if ((recivedChar >= '0') && (recivedChar <= '9')) {
        loggingFileSize = loggingFileSize + (recivedChar - '0');
        if (loggingFileSize < 1000000000) loggingFileSize = loggingFileSize * 10;
      }
    }
    if (loggingFileSize < 1000000000) loggingFileSize = loggingFileSize / 10;
    return loggingFileSize;

  }
  else {
    // Error:
    return 0;
  }
}

boolean openLog::waitForChar(char whichChar) { // Wait for the OL to give us the char 'whichChar'
  char recivedChar;
  timeOut = millis();
  while((millis() - timeOut) < 2000) {
    if ((*hardPort).available() > 0) {
      recivedChar = (*hardPort).read();
      if (whichChar == 1) {
        return true;
      }
      else {
        if (recivedChar == whichChar) {
          return true;
        }
      }
    }
  }
  return false;
}
