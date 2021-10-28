/********************************************************************************************************
  10kHz to 120MHz VFO / RF Generator with Si5351 and Arduino Nano, with Intermediate Frequency (IF)
  offset (+ or -). See the schematics for wiring details. By J. CesarSound - ver 1.0 - Dec/2020.
*********************************************************************************************************/

//Libraries
#include <Wire.h>                 // IDE Standard
#include <EEPROM.h>
#include <EncButton.h>            // https://github.com/GyverLibs/EncButton
#include <LiquidCrystal_I2C.h>    // by Frank de Brabander
#include "gen5351.h"
#include "gen9833.h"

int _memoryFree();
//#define Serial_print_memoryFree { Serial.print(_memoryFree()); Serial.print(" "); }
#define Serial_print_memoryFree (void)0 // disable memory free

void ClickF();
void RightF();
void LeftF();
void RightHoldF();
void LeftHoldF();
void HoldedF();

//-----------------------------------------------------------------------------
#define ENCCCW    2 // DIR_CCW pin
#define ENCCW     3 // DIR_CW pin
#define ENCBTN    4 // encoder push button

//-----------------------------------------------------------------------------
EncButton<EB_CALLBACK, ENCCCW, ENCCW, ENCBTN> enc;
LiquidCrystal_I2C *lcd = new LiquidCrystal_I2C(0x27, 16, 2);

gen5351 *g1;
gen9833 *g2;
genX *g;
bool g2mode = false;          // true if ad9833 is current

bool tick2name = true;
unsigned long tick2mill = 0;
#define TICK2_NAME 2000
#define TICK2_INFO 8000

#define PERIOD 100      // millis display active
unsigned long time_now = 0;  // millis display active

void setup() {
  Wire.begin();
  Serial.begin(9600);
    
  lcd->init();
  lcd->backlight();

  g1 = new gen5351();
  g1->init();

  g2 = new gen9833(7);
  g2->init();

  g = g1;
  g->welcome(lcd);
  
  enc.attach(CLICK_HANDLER, ClickF);
  enc.attach(RIGHT_HANDLER, RightF);
  enc.attach(LEFT_HANDLER, LeftF);
  enc.attach(RIGHT_H_HANDLER, RightHoldF);
  enc.attach(LEFT_H_HANDLER, LeftHoldF);
  enc.attach(HOLDED_HANDLER, HoldedF);

  g->showFreq(lcd);
}

void loop() {
  enc.tick();
 
  if (g->check_and_update()) {
    time_now = millis();
  }

  if ((time_now + PERIOD) > millis()) {
    g->showFreq(lcd);
  }

  if (tick2mill == 0 || (tick2mill + (tick2name?TICK2_NAME:TICK2_INFO)) < millis()) {
    tick2name = !tick2name;
    tick2mill = millis();
    g->showInfo(lcd, tick2name);
    
    Serial_print_memoryFree;
  }
}

void ClickF() {
  if (g2mode) {
    g2->cycleWaveType();
    g->showInfo(lcd, false);
  }

}
 
void tick2reset() {
  tick2mill = 0;
  tick2name = true;
}

void RightF() {
  g->change_freq(1);
  tick2reset();
}

void LeftF() {
  g->change_freq(-1);
  tick2reset();
}

void RightHoldF() {
  time_now = (millis() + 100);
  g->increment_fstep(1);
  tick2reset();
  delay(100);
}

void LeftHoldF() {
  time_now = (millis() + 100);
  g->increment_fstep(-1);
  tick2reset();
  delay(100);
}

void HoldedF() {
  g2mode = !g2mode;
  if (g2mode) {
    g = g2;
  } else {
    g = g1;
  }
  g->welcome(lcd);
  g->showFreq(lcd);
  tick2reset();
}

// Переменные, создаваемые процессом сборки,
// когда компилируется скетч
extern int __bss_end;
extern void *__brkval;
 
// Функция, возвращающая количество свободного ОЗУ (RAM)
int _memoryFree()
{
   int freeValue;
   if((int)__brkval == 0)
      freeValue = ((int)&freeValue) - ((int)&__bss_end);
   else
      freeValue = ((int)&freeValue) - ((int)__brkval);
   return freeValue;
}
