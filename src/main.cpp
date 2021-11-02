/********************************************************************************************************
  10kHz to 120MHz VFO / RF Generator with Si5351 and Arduino Nano, with Intermediate Frequency (IF)
  offset (+ or -). See the schematics for wiring details. By J. CesarSound - ver 1.0 - Dec/2020.
*********************************************************************************************************/

//Libraries
#include <Wire.h>                 // IDE Standard
//#include <EEPROM.h>
#include <EncButton.h>            // https://github.com/GyverLibs/EncButton
#include <LiquidCrystal_I2C.h>    // by Frank de Brabander
#include "gen5351.h"
#include "gen9833.h"

//-----------------------------------------------------------------------------
#define ENCCCW     2 // DIR_CCW pin
#define ENCCW      3 // DIR_CW pin
#define ENCBTN     4 // encoder push button

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

bool welcome = true;
bool update = true;
bool updateFreq = false;
bool updateEnabled = false;

void ClickF();
void turnHoldF();
void HoldedF();
void DoubleClickF();

void tick2reset() {
    tick2mill = 0;
    tick2name = true;
}

    void setup()
{
    pinMode(CLK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(FSYNC_PIN, OUTPUT);

    Wire.begin();
#ifdef _CHECK_MEMORY_FREE_
  Serial.begin(9600);
#endif

  lcd->init();
  lcd->backlight();

  g1 = new gen5351();
  g1->init();

  g2 = new gen9833(/*DATA_PIN, CLK_PIN, FSYNC_PIN*/);
  g2->init();

  g = g2;
  g2mode = true;
  welcome = true;
  update = true;
  
  enc.attach(CLICK_HANDLER, ClickF);
  enc.attach(TURN_H_HANDLER, turnHoldF);
  enc.attach(HOLDED_HANDLER, HoldedF);
  enc.attachClicks(2, DoubleClickF);
}

void loop()
{
    // тик вернёт отличное от нуля значение, если произошло событие:
    // 1 - left + turn
    // 2 - right + turn
    // 3 - leftH + turnH
    // 4 - rightH + turnH
    // 5 - click
    // 6 - held
    // 7 - step
    // 8 - press

    // опрос этих событий можно проводить в условии,
    // чтобы "не тратить время" на постоянный опрос в loop
    if (enc.tick()) {
        if (enc.turn()) {
            g->change_freq(enc.getDir());
            updateFreq = true;
            g->updateFreq();
            time_now = millis();
            updateFreq = false;
        }

        // в конце лучше вызвать resetState(), чтобы сбросить необработанные флаги!
        enc.resetState();
    }

    if (welcome) {
        g->welcome(lcd);
        time_now = millis();
        welcome = false;
    }
    
    if (update) {
        g->update();
        time_now = millis();
        tick2reset();
        update = false;
    }
    
    if (updateEnabled) {
        g->updateEnabled();
        time_now = millis();
        updateEnabled = false;
    }

    if ((time_now + PERIOD) > millis()) {
        g->showFreq(lcd);
    }

    if (tick2mill == 0 || (tick2mill + (tick2name?TICK2_NAME:TICK2_INFO)) < millis()) {
        tick2name = !tick2name;
        tick2mill = millis();
        g->showInfo(lcd, tick2name);
    }
}

void ClickF() {
  if (g2mode) {
    g2->cycleWaveType();
    update = true;
  }

}

void DoubleClickF() {
  g->changeEnabled();
  tick2reset();
  updateEnabled = true;
}

void turnHoldF() {
  g->change_fstep(enc.getDir());
  tick2reset();
}

void HoldedF() {
  g2mode = !g2mode;
  if (g2mode) {
    g = g2;
  } else {
    g = g1;
  }
  welcome = true;
  update = true;
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
