
#ifndef FVlib_OpenLog_h
  #define FVlib_OpenLog_h

  #include <Arduino.h>
  #include <SoftwareSerial.h>

  class openLog {
    public:
    // Constructor:
      openLog(HardwareSerial &_port);
      //openLog(SoftwareSerial &_port);
    // Functions:
      byte findLastLoggingSession(String loggingFileName);

    // Variables:
      byte lastLoggingSession = 255;


    private:
      HardwareSerial *hardPort;
      SoftwareSerial *softPort;
      String olCommand; // we construct here the command that will be passed to the OpenLog
      long fileSize(String fileName); // Returns the passed file size

  };

#endif
