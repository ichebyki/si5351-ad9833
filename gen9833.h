#ifndef GEN_AD9833_H
#define GEN_AD9833_H

#define FSYNC_PIN 7 			 // Define FSYNC_PIN for fast digital writes
#define FNC_PIN FSYNC_PIN	 // Define FNC_PIN for fast digital writes
#include <MD_AD9833.h>     
#include "genBase.h"

class gen9833: public genBase {
public:

  enum WaveType
  {
    SINE,      ///< Set output to a sine wave at selected frequency
    SQUARE1,   ///< Set output to a square wave at selected frequency
    SQUARE2,   ///< Set output to a square wave at half selected frequency
    TRIANGLE,  ///< Set output to a triangle wave at selected frequency
  };

  gen9833(uint8_t _FNCpin = FSYNC_PIN, uint32_t _referenceFrequency = 25000000UL) {
    FNCpin = _FNCpin;
    referenceFrequency = _referenceFrequency;
    mode = MD_AD9833::MODE_SINE;
  }

  void init() {
    ad9833 = new MD_AD9833(FNCpin);
    
    // This MUST be the first command after declaring the AD9833 object
    ad9833->begin();
    
    ad9833->setFrequency(MD_AD9833::CHAN_0, (float)freq);
    ad9833->setFrequency(MD_AD9833::CHAN_1, (float)freq);
    ad9833->setMode(MD_AD9833::MODE_SINE);
    ad9833->setPhase(MD_AD9833::CHAN_0, 0);
    ad9833->setPhase(MD_AD9833::CHAN_1, 0);
    ad9833->setActiveFrequency(MD_AD9833::CHAN_0);
    ad9833->setActivePhase(MD_AD9833::CHAN_0);
  }

  void changeEnabled() override {
    genBase::changeEnabled();
    ad9833->setMode(enabled ? mode : MD_AD9833::MODE_OFF);
  }

  void update() {
    ad9833->setFrequency(MD_AD9833::CHAN_0, (float)freq);
    ad9833->setFrequency(MD_AD9833::CHAN_1, (float)freq);
    ad9833->setMode(enabled ? mode : MD_AD9833::MODE_OFF);
    ad9833->setPhase(MD_AD9833::CHAN_0, 0);
    ad9833->setPhase(MD_AD9833::CHAN_1, 0);
    ad9833->setActiveFrequency(MD_AD9833::CHAN_0);
    ad9833->setActivePhase(MD_AD9833::CHAN_0);
  }

  void change_fstep(short dir = 1) override {
    genBase::change_fstep(dir);
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

  void showWave(LiquidCrystal_I2C *lcd) {
    lcd->setCursor(13, 0);
    lcd->print(enabled ? getWaveTypeName() : "OFF");
  }

  void showFreq(LiquidCrystal_I2C *lcd) override {
    genBase::showFreq(lcd);
    showWave(lcd);
  }
  
  void showInfo(LiquidCrystal_I2C *lcd, bool showName) override {
    genBase::showInfo(lcd, showName);
    showWave(lcd);
  }

  // SINE_WAVE, TRIANGLE_WAVE, SQUARE_WAVE, or HALF_SQUARE_WAVE
  void setWaveType(WaveType _waveType) {
    switch (_waveType) {
    case SINE: mode = MD_AD9833::MODE_SINE; break;
    case SQUARE1: mode = MD_AD9833::MODE_SQUARE1; break;
    case SQUARE2: mode = MD_AD9833::MODE_SQUARE2; break;
    case TRIANGLE: mode = MD_AD9833::MODE_TRIANGLE; break;
    }
    ad9833->setMode(enabled ? mode : MD_AD9833::MODE_OFF);
  }

  void cycleWaveType() {
    if (mode == MD_AD9833::MODE_SINE) {
      mode = MD_AD9833::MODE_TRIANGLE;
    } else if (mode == MD_AD9833::MODE_TRIANGLE) {
      mode = MD_AD9833::MODE_SQUARE1;
    } else if (mode == MD_AD9833::MODE_SQUARE1) {
      mode = MD_AD9833::MODE_SQUARE2;
    } else {
      mode = MD_AD9833::MODE_SINE;
    }
    ad9833->setMode(enabled ? mode : MD_AD9833::MODE_OFF);
  }
    
  WaveType getWaveType() {
    switch (mode) {
    case MD_AD9833::MODE_SINE: return(SINE);
    case MD_AD9833::MODE_SQUARE1: return(SQUARE1);
    case MD_AD9833::MODE_SQUARE2: return(SQUARE2);
    case MD_AD9833::MODE_TRIANGLE: return(TRIANGLE);
    default: return SINE;
    }
  }
    
  const char* getWaveTypeName() {
    switch(mode) {
    case MD_AD9833::MODE_SINE: return("SIN");
    case MD_AD9833::MODE_SQUARE1: return("SQ1");
    case MD_AD9833::MODE_SQUARE2: return("SQ2");
    case MD_AD9833::MODE_TRIANGLE: return("TRI");
    default: return "???";
    }
  }

  const char* name() override {
    return "ad9833";
  }
  
private:
  MD_AD9833 *ad9833;
  uint8_t FNCpin = FSYNC_PIN;
  uint32_t referenceFrequency = 25000000UL;
  MD_AD9833::mode_t mode = MD_AD9833::MODE_SINE;
};

#endif // GEN_9833_H
