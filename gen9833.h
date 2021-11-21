#ifndef GEN_AD9833_H
#define GEN_AD9833_H

// Pins for SPI comm with the AD9833 IC
#define FSYNC_PIN  7    // SPI Load pin number (FSYNC in AD9833 usage)
#define DATA_PIN   9    //11	// SPI Data pin number
#define CLK_PIN    8    //13	// SPI Clock pin number

#include <MD_AD9833.h>

#include "genBase.h"

#include "utils.h"

class gen9833: public genBase {
public:

    gen9833(LiquidCrystal_I2C *lcd, uint8_t _DATpin = DATA_PIN, uint8_t _CLKpin = CLK_PIN,
            uint8_t _FNCpin = FSYNC_PIN,
            uint32_t _referenceFrequency = 25000000UL) {
        this->lcd = lcd;
        FNCpin = _FNCpin;
        DATpin = _DATpin;
        CLKpin = _CLKpin;
        referenceFrequency = _referenceFrequency;
        mode = SINE;
    }

    void init() {
        ad9833 = new MD_AD9833(DATpin, CLKpin, FNCpin);
        //ad9833 = new MD_AD9833(FNCpin);
        
        // This MUST be the first command after declaring the AD9833 object
        ad9833->begin();
        update();
    }

    void update() {
        ad9833->reset(true);              // Reset and hold
        ad9833->setFrequency(MD_AD9833::CHAN_0, (float)freq);
        ad9833->setFrequency(MD_AD9833::CHAN_1, (float)freq);
        ad9833->setPhase(MD_AD9833::CHAN_0, 0);
        ad9833->setPhase(MD_AD9833::CHAN_1, 0);
        ad9833->reset();                  // full transition

        ad9833->setMode(getModeAD9833());
        ad9833->setActiveFrequency(MD_AD9833::CHAN_0);
        ad9833->setActivePhase(MD_AD9833::CHAN_0);
    }

    void updateFreq() {
        ad9833->setFrequency(MD_AD9833::CHAN_0, (float)freq);
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
        } else if (freq > 12000000) {
        freq = 12000000;
        }
    }

    void showMode() {
        lcd->setCursor(13, 0);
        lcd->print(enabled ? getModeName() : "OFF");
    }

    void showFreq() override {
        genBase::showFreq();
        showMode();
    }
    
    void showInfo(bool showName) override {
        genBase::showInfo(showName);
        showMode();
    }

    // SINE_WAVE, TRIANGLE_WAVE, SQUARE_WAVE, or HALF_SQUARE_WAVE
    void setMode(WaveType _waveType) override { mode = _waveType; }

    void cycleMode(short dir = 1) override {
        if (dir > 0) {
            if (mode == SINE) {
                mode = TRIANGLE;
            } else if (mode ==TRIANGLE) {
                mode =SQUARE1;
            } else if (mode ==SQUARE1) {
                mode =SQUARE2;
            } else {
                mode =SINE;
            }
        } else if (dir < 0) {
            if (mode ==SINE) {
                mode =SQUARE2;
            } else if (mode ==TRIANGLE) {
                mode =SINE;
            } else if (mode ==SQUARE1) {
                mode =TRIANGLE;
            } else {
                mode =SQUARE1;
            }
        }
    }

    WaveType getMode() override { return mode; }

    const char* getModeName() override {
        switch (mode) {
            case SINE:
                return ("SIN");
            case SQUARE1:
                return ("SQ1");
            case SQUARE2:
                return ("SQ2");
            case TRIANGLE:
                return ("TRI");
            case OFF:
                return ("OFF");
        }
        return "???";
    }

    MD_AD9833::mode_t getModeAD9833() {
        switch (mode) {
            case SINE:      return (MD_AD9833::MODE_SINE);
            case SQUARE1:   return (MD_AD9833::MODE_SQUARE1);
            case SQUARE2:   return (MD_AD9833::MODE_SQUARE2);
            case TRIANGLE:  return (MD_AD9833::MODE_TRIANGLE);
            case OFF:       return (MD_AD9833::MODE_OFF);
        }
        return MD_AD9833::MODE_OFF;
    }

    const char* name() override {
        return "ad9833";
    }
  
private:
    MD_AD9833 *ad9833;
    uint8_t FNCpin = FSYNC_PIN;
    uint8_t DATpin = DATA_PIN;
    uint8_t CLKpin = CLK_PIN;
    uint32_t referenceFrequency = 25000000UL;
};

#endif // GEN_9833_H
