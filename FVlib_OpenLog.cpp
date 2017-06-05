#include "FVlib_OpenLog.h"

// Delay OpenLog, needed for the OpenLog to process the buffer
  #define dOL 30 // Less than 30 will give problems 30ok


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
  if (!waitForChar('>')) return 1;


  lastLoggingSession = findLastLoggingSession(loggingFileName);
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
  delay(dOL);
  if ((char((*hardPort).read()) == '\r') && (char((*hardPort).read()) == '\n') && (char((*hardPort).read()) == '<')) {
    // We're in append mode now
    (*hardPort).print(textToAppend);
    delay(dOL);
  }
  else { // Append mode could not be reached
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

byte openLog::findLastLoggingSession(String loggingFileName) {
  byte index;
  long loggingFileSize;

  // Check file from 9 to 1
  for (index = 9; index != 0; index--) {
    olCommand = String(loggingFileName + String(index) + ".txt");
    loggingFileSize = fileSize(olCommand);
    Serial.print(olCommand);
    Serial.println(loggingFileSize);
    if ((loggingFileSize != 0) && (index == 9)) return 255; // No available logging sessions
    if (loggingFileSize != 0) return index; // Available logging session found
  }
  return 1; // Logging session '1' is the last one
}

long openLog::fileSize(String fileName) {
  // Return: 0:Error !=0:file size
  long loggingFileSize;
  char recivedChar;

  // Clear the buffer:
  (*hardPort).println("");
  if (!waitForChar('>')) return 0;

  // Find the size:
  loggingFileSize = 0;
  olCommand = String("size " + fileName);
  (*hardPort).println(olCommand);
  if (!waitForChar('>')) return 0;
  if (!waitForChar(NULL)) return 0;
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

boolean openLog::waitForChar(char whichChar) { // Wait for the OL to give us the char 'whichChar'
  char recivedChar;
  timeOut = millis();
  while((millis() - timeOut) < 2000) {
    if ((*hardPort).available() > 0) {
      recivedChar = (*hardPort).read();
      //Serial.print('.');
      if (whichChar == NULL) return true; // If we just received a char
      if (recivedChar == whichChar) {
        return true;
      }
    }
  }
  return false;
}
// ultim
