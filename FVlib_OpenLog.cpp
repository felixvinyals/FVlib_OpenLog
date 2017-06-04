#include "FVlib_OpenLog.h"

// Constructor:
openlog::serialConfig(HardwareSerial &_port) {
  hspSel = true;
  hardPort = &_port;
}
serialConfig::serialConfig(SoftwareSerial &_port) {
  hspSel = false;
  softPort = &_port;
}
