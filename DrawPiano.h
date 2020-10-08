#ifndef DRAWPIANO_H
#define DRAWPIANO_H

#ifdef ARDUINO_M5Stick_C
  #include <M5StickC.h>
#else
  #include <M5Stack.h>
#endif

class DrawPiano
{
  public:
    #ifdef _M5STACK_H_
      DrawPiano(int x = 0, int y = 50, int w =40, int h= 60); //for M5Stack _M5STACK_H_ M5Stack_Core_ESP32 M5STACK_FIRE
    #endif
    #ifdef _M5STICKC_H_
      DrawPiano(int x = 2, int y = 20, int w = 20, int h = 24); // for M5StickC _M5STICKC_H_ ESP32_PICO
    #endif

    void init();
    void drawKey(int num, bool boo, int keyOnColor = 64800); //64800 = ORANGE
  private:
    int keyX[13] = {0,1,2,3,4,6,7,8,9,10,11,12,14};
    int keyY[13] = {0,1,0,1,0,0,1,0,1,0,1,0,0};

    int posX, posY, keyWidth, keyHeight;
};

#endif
