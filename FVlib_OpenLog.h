
#ifndef FVlib_OpenLog_h
  #define FVlib_OpenLog_h

  #include <Arduino.h>
  #include <SoftwareSerial.h>

  class openLog {
    public:
    // Constructor:
      openLog(HardwareSerial &_port, byte vccPinOpenlLog);
    // Functions:
      byte appendToLastLoggingSession(String loggingFileName, String textToAppend, unsigned int _MBfileSizeLimit);
      byte doAppendToLastLoggingSession(String loggingFileName, String textToAppend, unsigned int _MBfileSizeLimit);
      byte findLastLoggingSession(String loggingFileName, unsigned int _MBfileSizeLimit); // Returns the index of the las logging session
      long fileSize(String fileName); // Returns the passed file size
    // Variables:



    private:
    // Functions:
      boolean waitForChar(char whichChar);
    // Variables:
      HardwareSerial *hardPort;
      SoftwareSerial *softPort;
      String olCommand; // we construct here the command that will be passed to the OpenLog
      byte lastLoggingSession = 255;
      unsigned long timeOut;

  };

#endif
