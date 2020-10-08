/*
 * https://github.com/lathoub/Arduino-BLE-MIDI
 * ESP32 BLE / NimBLE
 */

#include <M5Stack.h>
#include <Wire.h>
#include <M5StackUpdater.h> //M5Stack SD-Updater
#include "TouchPiano.h"
#include "DrawPiano.h"

#include <BLEMIDI.h>
//#include <hardware/BLEMIDI_ESP32_NimBLE.h>  // Bad
#include <hardware/BLEMIDI_ESP32.h>       // Good !

#define DEVICE_NAME               "M5Stack Piano"
//BLEMIDI_CREATE_DEFAULT_INSTANCE();
BLEMIDI_CREATE_INSTANCE(DEVICE_NAME, MIDI);  //●

bool isConnected = false;

int pos = 0;

bool touch[13];
bool keyStatus[13];
TouchPiano touchpiano;

DrawPiano drawpiano = DrawPiano(); // position: x,y key: width w, height h

const uint8_t buttonPins[3] = {39, 38, 37}; //C: 39, 37  /Stack:A, B, C=39, 38, 37
boolean buttonFlags[3];
uint8_t buttonNote[3]  = {60, 64, 67};
uint8_t buttonToKey[3] = {0, 4, 7};

//バッテリー表示
#define RGB(r,g,b) (int16_t)(b + (g << 5) + (r << 11)) //色0～31 緑0～63
unsigned long currentMillis;
unsigned long lastupdateMillis = 0;
int lastbattery = -1;
void displayBatteryLevel()
{
    if (currentMillis - lastupdateMillis > 1000) {
        int battlevel = 0;
        byte retval;
        Wire.beginTransmission(0x75);
        Wire.write(0x78);
        if (Wire.endTransmission(false) == 0 && Wire.requestFrom(0x75, 1)) {
            retval = Wire.read() & 0xF0;
            if (retval == 0xE0) battlevel = 25;
            else if (retval == 0xC0) battlevel = 50;
            else if (retval == 0x80) battlevel = 75;
            else if (retval == 0x00) battlevel = 100;
        }
        if (lastbattery != battlevel){
            M5.Lcd.fillRect(250, 5, 56, 21, RGB(31, 63, 31));
            M5.Lcd.fillRect(306, 9, 4, 13, RGB(31, 63, 31));
            M5.Lcd.fillRect(252, 7, 52, 17, RGB(0, 0, 0));
            if (battlevel <= 25)
                M5.Lcd.fillRect(253, 8, battlevel/2, 15, RGB(31, 20, 10));
            else
                M5.Lcd.fillRect(253, 8, battlevel/2, 15, RGB(20, 40, 31));
            lastbattery = battlevel;
        }
        lastupdateMillis = currentMillis;
    }
}

//BLE-MIDI
//BLE-MIDI送信

void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MIDI.sendNoteOn(pitch, velocity, 1);
}

void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MIDI.sendNoteOff(pitch, velocity, 1);
}

void setup() {
  M5.begin(); 
  Wire.begin(21, 22);// M5Stack SDA:21, SCL:22

  //M5Stack SD-Updater
  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  //M5Stack3VoiceSynth
  
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen( BLACK );
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.println(DEVICE_NAME);

  drawpiano.init();

  //BLE-MIDI  ●
  MIDI.begin();

  //受信
  BLEMIDI.setHandleConnected(OnConnected);
  BLEMIDI.setHandleDisconnected(OnDisconnected);
  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleNoteOff(OnNoteOff);
}


void loop() {
  // Piano Module
  bool t = touchpiano.getTouch(touch);
  if (t) {
    for (int counter = 0; counter < 13; counter++) {
      if (touch[counter]) {
        if (keyStatus[counter] == false) {
          noteOn(0, counter + 60, 127);
          drawpiano.drawKey(counter, true, RED); //
          keyStatus[counter] = true;
        }
      } else {
        if (keyStatus[counter] == true) {
          noteOff(0, counter + 60, 0);
          drawpiano.drawKey(counter, false);
          keyStatus[counter] = false;
        }
      }
    }
  }

  // M5Stack buttons
  for (int i = 0; i < 3; i++) { // 3: button
    int buttonValue = digitalRead(buttonPins[i]);
    //Serial.printf("%d : %d \n", i, buttonValue);
    if (buttonValue == LOW && buttonFlags[i] == false) {
      Serial.print("On: ");
      Serial.println(i);
      // noteOn
      noteOn(0, buttonToKey[i] + 60, 127);
      buttonFlags[i] = true;
      drawpiano.drawKey(buttonToKey[i], true);
    }
    if (buttonValue == HIGH && buttonFlags[i] == true) {
      Serial.print("Off: ");
      Serial.println(i);
      // noteOff
      noteOff(0, buttonToKey[i] + 60, 0);
      buttonFlags[i] = false;
      drawpiano.drawKey(buttonToKey[i], false);
    }
  }

  //バッテリー
  currentMillis = millis();
  displayBatteryLevel();

  delay(1);
  //M5.update();

  MIDI.read();  //■
}

// BLE-MIDI受信

// -----------------------------------------------------------------------------
// Device connected
// -----------------------------------------------------------------------------
void OnConnected() {
  isConnected = true;
  //digitalWrite(LED_BUILTIN, HIGH);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(140, 220);
  M5.Lcd.printf("Connected.   ");
}

// -----------------------------------------------------------------------------
// Device disconnected
// -----------------------------------------------------------------------------
void OnDisconnected() {
  isConnected = false;
  //digitalWrite(LED_BUILTIN, LOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(140, 220);
  M5.Lcd.printf("Disconnected.");
}

// -----------------------------------------------------------------------------
// Received note on
// -----------------------------------------------------------------------------
void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.println("Note On");
  M5.Lcd.setCursor(10, 190);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("%02x %02x %02x", 0x90 + channel, note, velocity);
}

// -----------------------------------------------------------------------------
// Received note off
// -----------------------------------------------------------------------------
void OnNoteOff(byte channel, byte note, byte velocity) {
  Serial.println("Note Off");
  M5.Lcd.setCursor(10, 190);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("%02x %02x %02x", 0x80 + channel, note, velocity);
}
