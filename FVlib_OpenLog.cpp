#include "FVlib_OpenLog.h"

// Delay OpenLog, needed for the OpenLog to process the buffer
  #define dOL 30 // Less than 30 will give problems 30ok


// Constructor:
openLog::openLog(HardwareSerial &_port, byte _vccPinOpenlLog, boolean _verboseMode) {
  hardPort = &_port;
  vccPinOpenlLog = _vccPinOpenlLog;
  verboseMode = _verboseMode;
  (*hardPort).begin(9600);
  pinMode(vccPinOpenlLog, OUTPUT);
  digitalWrite(vccPinOpenlLog, HIGH);
}

byte openLog::appendToLastLoggingSession(String loggingFileName, String textToAppend, unsigned int _MBfileSizeLimit) {
  // Return:
    // 0:Ok
    // 1:Error
    // 2:No available space
  byte appendResult = 1;

  for (byte attempts = 0; attempts < 3; attempts++) {
    appendResult = doAppendToLastLoggingSession(loggingFileName, textToAppend, _MBfileSizeLimit);
    if (appendResult == 0) return 0;
    else if (appendResult == 2) return 2;
    if (verboseMode) Serial.println("SD rebooted");
    digitalWrite(vccPinOpenlLog, LOW);
    delay(1000);
    digitalWrite(vccPinOpenlLog, HIGH);
    delay(1000);
  }
  if (verboseMode) Serial.print("Fail to work with SD");
  return 1; // Fail to comunicate with the SD
}

byte openLog::doAppendToLastLoggingSession(String _loggingFileName, String _textToAppend, unsigned int _MBfileSizeLimit) {
// Return:
  // 0:Ok
  // 1:Error
  // 2:No available space
  char recivedChar;

  // Initialize SD, useful if SD stopped working
  (*hardPort).println("init");
  if (!waitForChar('>')) return 1;
  lastLoggingSession = findLastLoggingSession(_loggingFileName, _MBfileSizeLimit);
  if (lastLoggingSession == 255) return 1;
  if (lastLoggingSession == 254) return 2;

  olCommand = String("append " + _loggingFileName + lastLoggingSession + ".txt");

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
    (*hardPort).print(_textToAppend);
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
  // 1 to 9: logging session
  // 254: No available logging sessions
  // 255: Error

  byte index;
  unsigned long loggingFileSize;
  unsigned long bytesFileSizeLimit =  _MBfileSizeLimit * 1048576; // 1048576bytes = 1Mb

  if (verboseMode) {
    Serial.print("File Size Limit in bytes: ");
    Serial.println(bytesFileSizeLimit);
  }
  // Check file from 9 to 1
  for (index = 9; index != 0; index--) {
    olCommand = String(loggingFileName + String(index) + ".txt");
    loggingFileSize = fileSize(olCommand);
    if (verboseMode) {
      Serial.print(olCommand);
      Serial.print(" --> ");
      Serial.print(loggingFileSize);
      Serial.print(" bytes");
    }
    if (loggingFileSize == 4294967295) return 255; // Report Error
    else if ((index == 9) && (loggingFileSize >= bytesFileSizeLimit)) {
      if (verboseMode) Serial.println(" --> Case: 1");
      return 254; // No available logging sessions
    }
    else if ((index == 1) && (loggingFileSize >= bytesFileSizeLimit)) {
      if (verboseMode) Serial.println(" --> Case: 2");
      return ++index;
    }
    else if ((index == 1) && (loggingFileSize < bytesFileSizeLimit)) {
      if (verboseMode) Serial.println(" --> Case: 3");
      return index;
    }
    else if ((loggingFileSize != 0) && (loggingFileSize < bytesFileSizeLimit)) {
      if (verboseMode) Serial.println(" --> Case: 4");
      return index;
    }
    else if (loggingFileSize == 0) {
      if (verboseMode) Serial.println(" --> Case: 5");
    }
    else if (loggingFileSize >= bytesFileSizeLimit) {
      if (verboseMode) Serial.println(" --> Case: 6");
      return ++index;
    }
  }
  return 255; // Report Error
}

unsigned long openLog::fileSize(String fileName) {
// Return:
  // 0: File does not exist
  // 4294967295: Error (we use this weird number because is the largest that a long can handle. Any file will have this size)
  // else: File Size
  unsigned long loggingFileSize;
  char recivedChar;

  // Clear the buffer:
  (*hardPort).println("");
  if (!waitForChar('>')) return 4294967295;

  // Find the size:
  olCommand = String("size " + fileName);
  (*hardPort).println(olCommand);
  delay(dOL);
  loggingFileSize = 0;

  if (waitForChar('\n') == true) {
    while((*hardPort).available() > 0) {
      recivedChar = (*hardPort).read();
      if (recivedChar == '-') {
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
  else { // Error:
    return 4294967295;
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
