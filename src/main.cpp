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

//#define _SERIAL_LOG_

//-----------------------------------------------------------------------------
#define ENCCCW     2 // DIR_CCW pin
#define ENCCW      3 // DIR_CW pin
#define ENCBTN     4 // encoder push button

//-----------------------------------------------------------------------------
//EncButton<EB_CALLBACK, ENCCCW, ENCCW, ENCBTN> enc;
EncButton<EB_TICK, ENCCCW, ENCCW, ENCBTN> enc;
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
#ifdef _SERIAL_LOG_
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
        if (enc.turn()) // обычный поворот
        {
            updateFreq = true;
            g->change_freq(enc.getDir());
            g->updateFreq();
            time_now = millis();
            updateFreq = false;
        }
        else if (enc.turnH()) 
        {
            g->change_fstep(enc.getDir());
            tick2reset();
        }
        else if (enc.click())
        {
            if (enc.clicks == 1)
            {
                if (g2mode)
                {
                    update = true;
                    g2->cycleWaveType();
                    g->update();
                    time_now = millis();
                    tick2reset();
                    update = false;
                }
            }
            if (enc.clicks==2)
            {
                updateEnabled = true;
                g->changeEnabled();
                g->updateEnabled();
                time_now = millis();
                tick2reset();
                updateEnabled = false;
            }
        }
        else if (enc.held()) // однократно вернёт true при удержании
        {
            g2mode = !g2mode;
            if (g2mode)
            {
                g = g2;
            }
            else
            {
                g = g1;
            }
            welcome = true;
            update = true;
        }

        // в конце лучше вызвать resetState(), чтобы сбросить необработанные флаги!
        //enc.resetState();
    }

    if (welcome)
    {
        g->welcome(lcd);
        time_now = millis();
        welcome = false;
    }

    if (update)
    {
        g->update();
        time_now = millis();
        tick2reset();
        update = false;
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
