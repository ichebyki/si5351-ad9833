/********************************************************************************************************
  10kHz to 120MHz VFO / RF Generator with Si5351 and Arduino Nano, with
Intermediate Frequency (IF) offset (+ or -). See the schematics for wiring
details. By J. CesarSound - ver 1.0 - Dec/2020.
*********************************************************************************************************/

// Libraries
#include <Wire.h>  // IDE Standard
//#include <EEPROM.h>
#include <EncButton.h>          // https://github.com/GyverLibs/EncButton
#include <LiquidCrystal_I2C.h>  // by Frank de Brabander

#include "gen5351.h"
#include "gen9833.h"
#include "genMenu.h"

//#define _SERIAL_LOG_

//-----------------------------------------------------------------------------
#define ENCCCW 2  // DIR_CCW pin
#define ENCCW 3   // DIR_CW pin
#define ENCBTN 4  // encoder push button

//-----------------------------------------------------------------------------
// EncButton<EB_CALLBACK, ENCCCW, ENCCW, ENCBTN> enc;
EncButton<EB_TICK, ENCCCW, ENCCW, ENCBTN> enc;
LiquidCrystal_I2C _lcd(0x27, 16, 2);
LiquidCrystal_I2C *lcd = &_lcd;

gen5351 _g1;
genMenu _m1(lcd, 2);
gen9833 _g2;  //(/*DATA_PIN, CLK_PIN, FSYNC_PIN*/);
genMenu _m2(lcd, 5);
gen5351 *g1 = (gen5351 *)&_g1;
genMenu *m1 = &_m1;
gen9833 *g2 = (gen9833 *)&_g2;
genMenu *m2 = &_m2;
genBase *g = g1;
bool g2mode = false;  // true if ad9833 is current

bool tick2name = true;
unsigned long tick2mill = 0;
#define TICK2_NAME 2000
#define TICK2_INFO 8000

#define PERIOD 100           // millis display active
unsigned long time_now = 0;  // millis display active

bool welcome = true;
bool update = true;
bool updateFreq = false;
bool updateEnabled = false;

bool menumode = false;

void tick2reset() {
    tick2mill = 0;
    tick2name = true;
}

void setup() {
    pinMode(CLK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(FSYNC_PIN, OUTPUT);

    Wire.begin();
#ifdef _SERIAL_LOG_
    Serial.begin(9600);
#endif

    lcd->init();
    lcd->backlight();

    g1->init();

    g2->init();

    g = g2;
    g2mode = true;
    welcome = true;
    update = true;

    m2->setItem(0, (short)gen9833::WaveType::SINE, "SIN");
    m2->setItem(1, (short)gen9833::WaveType::TRIANGLE, "TRI");
    m2->setItem(2, (short)gen9833::WaveType::SQUARE1, "SQ1");
    m2->setItem(3, (short)gen9833::WaveType::SQUARE2, "SQ2");
    m2->setItem(4, (short)gen9833::WaveType::OFF, "OFF");

    m1->setItem(0, (short)1, "ON");
    m1->setItem(1, (short)0, "OFF");
}

void loop() {
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
#ifdef _SERIAL_LOG_
    Serial.print("_memoryFree = ");
    Serial.println(_memoryFree());
#endif
    if (enc.tick()) {
        if (menumode) {
            if (enc.turn()) {  // --------------- обычный поворот
                if (g2mode) {
                    m2->cycleCurrent(enc.getDir());
                } else {
                    m1->cycleCurrent(enc.getDir());
                }
                tick2reset();
            } else if (enc.click()) {
                if (g2mode) {
                    g2->setWaveType(
                        (gen9833::WaveType)m2->getCurrentItem()->val);
                } else {
                    g1->setEnabled(m1->getCurrentItem()->val == 1 ? true
                                                                  : false);
                }
                menumode = false;
                update = true;
            }
        } else {
            if (enc.turn()) {  // --------------- обычный поворот
                updateFreq = true;
                g->change_freq(enc.getDir());
                g->updateFreq();
                time_now = millis();
                updateFreq = false;
            } else if (enc.turnH()) {  // --------------- поворот с нажатием
                g->change_fstep(enc.getDir());
                tick2reset();
            } else if (enc.click()) {  // --------------- клик
                menumode = true;
                tick2reset();
            } else if (enc.held()) {  // --------------- однократно вернёт true
                                      // при удержании
                g2mode = !g2mode;
                if (g2mode) {
                    g = g2;
                } else {
                    g = g1;
                }
                welcome = true;
                update = true;
            }
        }

        // в конце лучше вызвать resetState(), чтобы сбросить необработанные
        // флаги!
        // enc.resetState();
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

    if (!menumode) {
        if ((time_now + PERIOD) > millis()) {
            g->showFreq(lcd);
        }
    }

    if (tick2mill == 0 ||
        (tick2mill + (tick2name ? TICK2_NAME : TICK2_INFO)) < millis()) {
        tick2name = !tick2name;
        tick2mill = millis();
        if (menumode) {
            if (g2mode) {
                m2->show();
            } else {
                m1->show();
            }
        } else {
            g->showInfo(lcd, tick2name);
        }
    }
}

// Переменные, создаваемые процессом сборки,
// когда компилируется скетч
extern int __bss_end;
extern void *__brkval;

// Функция, возвращающая количество свободного ОЗУ (RAM)
int _memoryFree() {
    int freeValue;
    if ((int)__brkval == 0)
        freeValue = ((int)&freeValue) - ((int)&__bss_end);
    else
        freeValue = ((int)&freeValue) - ((int)__brkval);
    return freeValue;
}
