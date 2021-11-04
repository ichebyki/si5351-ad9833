#ifndef GEN_AD9833_H
#define GEN_AD9833_H

// Pins for SPI comm with the AD9833 IC
#define FSYNC_PIN  7    // SPI Load pin number (FSYNC in AD9833 usage)
#define DATA_PIN   9    //11	// SPI Data pin number
#define CLK_PIN    8    //13	// SPI Clock pin number

#include <MD_AD9833.h>     
#include "genBase.h"

class gen9833: public genBase {
public:
    enum WaveType {
        SINE,      ///< Set output to a sine wave at selected frequency
        TRIANGLE,  ///< Set output to a triangle wave at selected frequency
        SQUARE1,   ///< Set output to a square wave at selected frequency
        SQUARE2,   ///< Set output to a square wave at half selected frequency
        OFF
    };

    gen9833(LiquidCrystal_I2C *lcd, uint8_t _DATpin = DATA_PIN, uint8_t _CLKpin = CLK_PIN,
            uint8_t _FNCpin = FSYNC_PIN,
            uint32_t _referenceFrequency = 25000000UL) {
        this->lcd = lcd;
        FNCpin = _FNCpin;
        DATpin = _DATpin;
        CLKpin = _CLKpin;
        referenceFrequency = _referenceFrequency;
        mode = MD_AD9833::MODE_SINE;
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
        
        ad9833->setMode(enabled ? mode : MD_AD9833::MODE_OFF);
        ad9833->setActiveFrequency(MD_AD9833::CHAN_0);
        ad9833->setActivePhase(MD_AD9833::CHAN_0);
    }

    void updateFreq() {
        ad9833->setFrequency(MD_AD9833::CHAN_0, (float)freq);
    }

    void updateEnabled() {
        ad9833->setMode(enabled ? mode : MD_AD9833::MODE_OFF);
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

    void showWave() {
        lcd->setCursor(13, 0);
        lcd->print(enabled ? getWaveTypeName() : "OFF");
    }

    void showFreq() override {
        genBase::showFreq();
        showWave();
    }
    
    void showInfo(bool showName) override {
        genBase::showInfo(showName);
        showWave();
    }

    // SINE_WAVE, TRIANGLE_WAVE, SQUARE_WAVE, or HALF_SQUARE_WAVE
    void setWaveType(WaveType _waveType) {
        switch (_waveType) {
        case SINE: mode = MD_AD9833::MODE_SINE; break;
        case TRIANGLE: mode = MD_AD9833::MODE_TRIANGLE; break;
        case SQUARE1: mode = MD_AD9833::MODE_SQUARE1; break;
        case SQUARE2: mode = MD_AD9833::MODE_SQUARE2; break;
        case OFF: mode = MD_AD9833::MODE_OFF; break;
        }
    }

    void cycleWaveType(short dir = 1) {
        if (dir > 0) {
            if (mode == MD_AD9833::MODE_SINE) {
                mode = MD_AD9833::MODE_TRIANGLE;
            } else if (mode == MD_AD9833::MODE_TRIANGLE) {
                mode = MD_AD9833::MODE_SQUARE1;
            } else if (mode == MD_AD9833::MODE_SQUARE1) {
                mode = MD_AD9833::MODE_SQUARE2;
            } else {
                mode = MD_AD9833::MODE_SINE;
            }
        } else if (dir < 0) {
            if (mode == MD_AD9833::MODE_SINE) {
                mode = MD_AD9833::MODE_SQUARE2;
            } else if (mode == MD_AD9833::MODE_TRIANGLE) {
                mode = MD_AD9833::MODE_SINE;
            } else if (mode == MD_AD9833::MODE_SQUARE1) {
                mode = MD_AD9833::MODE_TRIANGLE;
            } else {
                mode = MD_AD9833::MODE_SQUARE1;
            }
        }
    }

    WaveType getWaveType() {
        switch (mode) {
        case MD_AD9833::MODE_SINE: return(SINE);
        case MD_AD9833::MODE_SQUARE1: return(SQUARE1);
        case MD_AD9833::MODE_SQUARE2: return(SQUARE2);
        case MD_AD9833::MODE_TRIANGLE: return(TRIANGLE);
        case MD_AD9833::MODE_OFF: return(OFF);
        default: return SINE;
        }
    }
        
    const char* getWaveTypeName() {
        switch(mode) {
        case MD_AD9833::MODE_SINE: return("SIN");
        case MD_AD9833::MODE_SQUARE1: return("SQ1");
        case MD_AD9833::MODE_SQUARE2: return("SQ2");
        case MD_AD9833::MODE_TRIANGLE: return("TRI");
        case MD_AD9833::MODE_OFF: return("OFF");
        default: return "???";
        }
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
    MD_AD9833::mode_t mode = MD_AD9833::MODE_SINE;
};

#endif // GEN_9833_H
