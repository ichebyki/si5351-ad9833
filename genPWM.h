#ifndef GEN_PWM_H
#define GEN_PWM_H

#include "utils.h"

#include "genBase.h"

class genPWM : public genBase {
   public:
    genPWM(LiquidCrystal_I2C* lcd, uint32_t _clockFrequency = 16000000UL) {
        this->lcd = lcd;
        clockFrequency = _clockFrequency;
        max1 = (uint32_t)((clockFrequency - 1) / 4);
        min1 = (uint32_t)(clockFrequency / 65536) + 1;
        max8 = (uint32_t)((clockFrequency - 1) / 8 / 4);
        min8 = (uint32_t)(clockFrequency / 8 / 65536) + 1;
    }

    void init() {
        // pinMode(9,OUTPUT);
        pinMode(10, OUTPUT);
        update();
    }

    void update() { updateFreq(); }

    void updateFreq() {
        // PWM is off
        sp("enabled = ", enabled);
        if (!enabled) {
            TCCR1A &= ~((1 << COM1A1) | (1 << COM1B1));
            return;
        }

        uint32_t top, duty, tmp;
        uint16_t prescaler = 1;
        
        if (freq < min8) { 
            freq = min8;
            prescaler = 8;
        } else if (freq < min1) {
            prescaler = 8;
        } else if (freq > max1) {
            freq = max1;
            prescaler = 1;
        } else {
            prescaler = 1;
        }

        top = clockFrequency / prescaler / freq - 1;
        if (top > 65535) {
            top = 65535;
        }
        // duty = max(1, top / 2); // ~50%
        tmp = (uint32_t)((float)(top + 1) * ((float)PWM / (float)100));
        duty = max(1, tmp);
        freq = clockFrequency / prescaler / (1 + top);

        // Clear OC1A on Compare Match / Set OC1A at Bottom; Wave Form
        // Generator: Fast PWM 14, Top = ICR1
        //TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); //  pin 9 and 10
        TCCR1A = (1 << COM1B1) + (1 << WGM11); // pin 10

        TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << (prescaler==1 ? CS10 : CS11));  // prescaler
        ICR1 = (uint16_t)top;
        //OCR1A = (uint16_t)duty;  //  pin 9
        OCR1B = (uint16_t)duty;  //  pin 10

        // DDRB |= (1 << PB2);
        // DDRB |= (1<<PB1)|(1<<PB2); // add pin 9
    }

    void change_fstep(short dir = 1) override {
        genBase::change_fstep(dir);
        if (dir > 0) {
            if (fstep > 1000000) {
                fstep = 1;
            }
        }
        if (fstep > 1000000) {
            fstep = 1000000;
        }
    }

    void change_freq(short dir) override {
        genBase::change_freq(dir);

        if (freq < min8) {
            freq = min8;
        } else if (freq > max1) {
            freq = max1;
        }
    }

    void showMode() {
        lcd->setCursor(16 - 6, 0);
        int tmp = (int)(PWM * 10.0);
        lcd->print(tmp / 10);
        lcd->print(".");
        lcd->print(tmp - (tmp / 10) * 10);
        lcd->print("%");
    }

    void showFreq() override {
        genBase::showFreq();
        showMode();
    }

    void showInfo(bool showName) override {
        genBase::showInfo(showName);
        showMode();
    }

    short getPWM() {
        return PWM;
    }

    short setPWM(short pwm) {
        if (pwm > 0 && pwm < 100) {
            PWM = pwm;
        }
        return PWM;
    }

    const char* name() override { return "Arduino PWM"; }

   private:
    uint32_t clockFrequency = 16000000UL;
    uint32_t min1;
    uint32_t max1;
    uint32_t min8;
    uint32_t max8;
    float PWM = 50;
};

#endif  // GEN_PWM_H