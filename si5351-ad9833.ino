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

//#define _CHECK_MEMORY_FREE_
#ifdef _CHECK_MEMORY_FREE_
  int _memoryFree();
  #define Serial_print_memoryFree { Serial.print(_memoryFree()); Serial.print(" "); }
#else
  #define Serial_print_memoryFree (void)0 // disable memory free
#endif

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
genBase *g;
bool g2mode = false;          // true if ad9833 is current

bool tick2name = true;
unsigned long tick2mill = 0;
#define TICK2_NAME 2000
#define TICK2_INFO 8000

#define PERIOD 100      // millis display active
unsigned long time_now = 0;  // millis display active

void setup() {
  Wire.begin();
#ifdef _CHECK_MEMORY_FREE_
  Serial.begin(9600);
#endif

  lcd->init();
  lcd->backlight();

  g1 = new gen5351();
  g1->init();

  g2 = new gen9833(7);
  g2->init();

  g = g1;
  g->welcome(lcd);
  
  enc.attach(CLICK_HANDLER, ClickF);
  enc.attach(TURN_HANDLER, turnF);
  enc.attach(TURN_H_HANDLER, turnHoldF);
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

void tick2reset() {
  tick2mill = 0;
  tick2name = true;
}

void ClickF() {
  if (g2mode) {
    g2->cycleWaveType();
    g->showInfo(lcd, false);
  }

}
 
void turnF() {
  g->change_freq(enc.getDir());
  tick2reset();
}

void turnHoldF() {
  time_now = (millis() + 100);
  g->change_fstep(enc.getDir());
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
