#include "TouchPiano.h"
#include <Wire.h>

enum TP_PIANO{
  None = 0x0000,
  C = 0x0001,
  bD = 0x0002,
  D = 0x0004,
  bE = 0x0008,
  E = 0x0010,
  F = 0x0020,
  bG = 0x0040,
  G = 0x0080,
  bA = 0x0100,
  A = 0x0200,
  bB = 0x0400,
  B = 0x0800,
  C1 = 0x1000
};

bool TouchPiano::getTouch(bool status[13]){
      Wire.requestFrom(0x57, 2);
      unsigned int  val1 = Wire.read();
      unsigned int  val2 = Wire.read();
      unsigned int  TPval = val1 + (val2 << 8);
      //Serial.println(TPval);
    
      if(TPval == 4294967039){
        return false;
      }
      for (int i=0; i < 13; i++){
        status[i] = false;
      }
      if (TPval & TP_PIANO::C) {
        status[0] = true;
      }
      if (TPval & TP_PIANO::bD) {
        status[1] = true;
      }
      if (TPval & TP_PIANO::D) {
        status[2] = true;
      }
      if (TPval & TP_PIANO::bE) {
        status[3] = true;
      }
      if (TPval & TP_PIANO::E) {
        status[4] = true;
      }
      if (TPval & TP_PIANO::F) {
        status[5] = true;
      }
      if (TPval & TP_PIANO::bG) {
        status[6] = true;
      }
      if (TPval & TP_PIANO::G) {
        status[7] = true;
      }
      if (TPval & TP_PIANO::bA) {
        status[8] = true;
      }
      if (TPval & TP_PIANO::A) {
        status[9] = true;
      }
      if (TPval & TP_PIANO::bB) {
        status[10] = true;
      }
      if (TPval & TP_PIANO::B) {
        status[11] = true;
      }
      if (TPval & TP_PIANO::C1) {
        status[12] = true;
      }
        return true;
}
