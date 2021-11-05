#ifndef GEN_PWM_H
#define GEN_PWM_H

#include "genBase.h"

class genPWM: public genBase {
public:
    genPWM(LiquidCrystal_I2C *lcd, uint32_t _clockFrequency = 16000000UL) {
        this->lcd = lcd;
        clockFrequency = _clockFrequency;
    }

    void init() {
        //pinMode(9,OUTPUT);
        pinMode(10,OUTPUT);
        TCCR1A=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);
        TCCR1B=(1<<WGM13)|(1<<WGM12)|(1<<CS10); //mode14 FastPwm

        ICR1=16;
        OCR1A=8;
        OCR1B=8;

        update();
    }

    void update() {
        updateFreq();
    }

    void updateFreq() {
        if (freq < 224) {
            freq = 224;
        }
        TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);
        TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS10); //mode14 FastPwm
        
        ICR1 = (uint16_t) ( (unsigned long)16000000 / (unsigned long)freq);
        OCR1A = (uint16_t) (ICR1 / 2 );
        OCR1B = (uint16_t) (ICR1 / 2 );

        freq = 16000000 / ICR1;
    }

    void updateEnabled() {
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

        if (freq < 1) {
            freq = 1;
        } else if (freq > 8000000) {
            freq = 8000000;
        }
    }

    void showMode() {
    }

    void showFreq() override {
        genBase::showFreq();
        showMode();
    }
    
    void showInfo(bool showName) override {
        genBase::showInfo(showName);
        showMode();
    }

    const char* name() override {
        return "Arduino PWM";
    }
  
private:
    uint32_t clockFrequency = 16000000UL;
};

#endif // GEN_PWM_H
