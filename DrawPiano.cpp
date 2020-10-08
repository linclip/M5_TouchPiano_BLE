#include "DrawPiano.h"

DrawPiano::DrawPiano(int x, int y, int w, int h)
{
  posX = x;
  posY = y;
  keyWidth = w;
  keyHeight = h;
}

void DrawPiano::init()
{
  for (int i=0; i < 13; i++){
    int x = keyX[i];
    int y = keyY[i];
    if (y){
      M5.Lcd.drawRect(posX + x * (keyWidth /2 - 1), (posY + keyHeight) - (y *  keyHeight), keyWidth -6, keyHeight -4 , WHITE);
      M5.Lcd.fillRect(posX + x * (keyWidth /2 -1) + 1 , (posY + keyHeight) - (y * keyHeight) + 1, keyWidth -8, keyHeight -6, BLACK);
    } else {
      M5.Lcd.fillRect(posX + x * (keyWidth /2 - 1), (posY + keyHeight) - (y *  keyHeight), keyWidth -6, keyHeight -4 , WHITE);
    }
  }
}

void DrawPiano::drawKey(int num, bool keyon, int keyonColor)
{
    int x = keyX[num];
    int y = keyY[num];
    
    if(keyon){
      M5.Lcd.fillRect(posX + x * (keyWidth /2 -1) + 1 , (posY + keyHeight) - (y * keyHeight) + 1, keyWidth -8, keyHeight -6, keyonColor); //note on
    } else {                                                         //note off
      if(y){
        M5.Lcd.fillRect(posX + x * (keyWidth /2 -1) + 1 , (posY + keyHeight) - (y * keyHeight) + 1, keyWidth -8, keyHeight -6, BLACK);
        } else {
        M5.Lcd.fillRect(posX + x * (keyWidth /2 -1) + 1 , (posY + keyHeight) - (y * keyHeight) + 1, keyWidth -8, keyHeight -6, WHITE);
      }
    }
}
