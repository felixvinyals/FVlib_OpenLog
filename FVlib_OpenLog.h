
#ifndef FVlib_OpenLog_h
  #define FVlib_OpenLog_h

  #include <Arduino.h>
  #include <SoftwareSerial.h>

  class openLog {
    public:
    // Constructor:
      openLog(HardwareSerial &_port, unsigned int MBfileSizeLimit);
      //openLog(SoftwareSerial &_port);
    // Functions:
<<<<<<< HEAD
      byte appendToLastLoggingSession(String loggingFileName, String textToAppend, unsigned int _MBfileSizeLimit);
      byte doAppendToLastLoggingSession(String loggingFileName, String textToAppend, unsigned int _MBfileSizeLimit);
=======
      byte appendToLastLoggingSession(String loggingFileName, String textToAppend); 
      byte findLastLoggingSession(String loggingFileName); // Returns the index of the las logging session
      long fileSize(String fileName); // Returns the passed file size
>>>>>>> parent of c3eab5c... Finished
    // Variables:



    private:
    // Functions:

<<<<<<< HEAD
      byte findLastLoggingSession(String loggingFileName, unsigned int _MBfileSizeLimit); // Returns the index of the las logging session
      long fileSize(String fileName); // Returns the passed file size
=======
>>>>>>> parent of c3eab5c... Finished
      boolean waitForChar(char whichChar);
    // Variables:
      HardwareSerial *hardPort;
      SoftwareSerial *softPort;
      String olCommand; // we construct here the command that will be passed to the OpenLog
      byte lastLoggingSession = 255;
      unsigned long timeOut;

  };

#endif
