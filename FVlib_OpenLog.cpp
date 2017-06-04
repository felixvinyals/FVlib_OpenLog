#include "FVlib_OpenLog.h"

#define dOL 15 // The delay we give to the OpenLog to process the buffer

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
  long loggingFileSize;
  char recivedChar;

  //olCommand = String("size test.txt");
  //(*hardPort).println(olCommand);
  (*hardPort).println("");
  delay(dOL);
  while((*hardPort).available() > 0) Serial.write((*hardPort).read()); // Clean the buffered
  delay(1000);

  for (index = 1; index <= 10; index++) {
    olCommand = String("size " + loggingFileName + String(index) + ".txt");
    (*hardPort).println(olCommand);
    Serial.print("Comprovant fitxer: ");
    Serial.println(index);
    delay(dOL);
    loggingFileSize = 0;
    if (((*hardPort).read() == '\r') && ((*hardPort).read() == '\n')) {
      while((*hardPort).available() > 0) {
        recivedChar = (*hardPort).read();
        Serial.write(recivedChar);
        if ((recivedChar == '!') || (recivedChar == '-')) {
          Serial.println("trobat !");
          break;
        }
        else if ((recivedChar >= '0') && (recivedChar <= '9')) {
          loggingFileSize = loggingFileSize + (recivedChar - '0');
          loggingFileSize = loggingFileSize * 10;
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
        loggingFileSize = loggingFileSize / 10;
        Serial.println(loggingFileSize);
      }
    }
    else {
      Serial.print(olCommand);
      Serial.println(" Saltat!");
    }
  }
  return 0;

/*
  for(conta = 100; conta >= 1; conta--) {
        Serial2.println("");
        while(Serial2.available() > 0) {
          lectura = Serial2.read();
          delay(15);
        }
        comandaSD = String("size CSO" + String(conta) + ".txt");
        Serial2.println(comandaSD);
        Serial.println(comandaSD);
        delay(15);
        tamanyFitxer = 0;
        while(Serial2.read() != '\n');
        delay(15);
        while(Serial2.available() > 0) {
          lectura = Serial2.read();
          if (lectura == '\r') {
            Serial2.read(); // Treiem del bufer el \n
            break;
          }
          else if (lectura >= '0' && lectura <= '9') {
            tamanyFitxer = tamanyFitxer + (lectura - '0');
            tamanyFitxer = tamanyFitxer * 10;
          }
          else {
            Serial.print(lectura);
          }
          delay(15);
        }
        tamanyFitxer = tamanyFitxer / 10;
        Serial.print("Tamany del fitxer ");
        Serial.print(conta);
        Serial.print(": ");
        Serial.println(tamanyFitxer);
        Serial.println("-----------");
        if (tamanyFitxer != 0) break;
      }
      Serial.println("Ultim fitxer lliure:");
      Serial.println(conta);
      while(1);
*/
}
