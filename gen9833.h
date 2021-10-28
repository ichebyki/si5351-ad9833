#ifndef GEN_AD9833_H
#define GEN_AD9833_H

#include <AD9833.h>               // https://github.com/Billwilliams1952/AD9833-Library-Arduino 
#include "genBase.h"

class gen9833: public genBase {
public:
  gen9833(uint8_t _FNCpin, uint32_t _referenceFrequency = 25000000UL) {
    FNCpin = _FNCpin;
    referenceFrequency = _referenceFrequency;
  }

  void init() {
    ad9833 = new AD9833(FNCpin, referenceFrequency);
    
    // This MUST be the first command after declaring the AD9833 object
    ad9833->Begin();              

    // Apply a 1000 Hz sine wave using REG0 (register set 0). There are two register sets,
    // REG0 and REG1. 
    // Each one can be programmed for:
    //   Signal type - SINE_WAVE, TRIANGLE_WAVE, SQUARE_WAVE, and HALF_SQUARE_WAVE
    //   Frequency - 0 to 12.5 MHz
    //   Phase - 0 to 360 degress (this is only useful if it is 'relative' to some other signal
    //           such as the phase difference between REG0 and REG1).
    // In ApplySignal, if Phase is not given, it defaults to 0.
    ad9833->ApplySignal(waveType,
                        freqReg,
                        freq,
                        phaseReg,
                        phaseInDeg);
   
    ad9833->EnableOutput(true);   // Turn ON the output - it defaults to OFF
    // There should be a 1000 Hz sine wave on the output of the AD9833
  }

  void update() {
    // Setup and apply a signal. Note that any calls to EnableOut,
    // SleepMode, DisableDAC, or DisableInternalClock remain in effect
    // void ApplySignal ( WaveformType waveType,
    //                    Registers freqReg,
    //                    float frequencyInHz,
    //                    Registers phaseReg = SAME_AS_REG0,
    //                    float phaseInDeg = 0.0 ); 
    ad9833->ApplySignal(waveType,
                        freqReg,
                        freq,
                        phaseReg,
                        phaseInDeg);
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

  void showFreq(LiquidCrystal_I2C *lcd) override {
    genBase::showFreq(lcd);
  }
  
  void showInfo(LiquidCrystal_I2C *lcd, bool showName) override {
    lcd->setCursor(13, 0);
    lcd->print(getWaveTypeName());
    genBase::showInfo(lcd, showName);
  }

  // SINE_WAVE, TRIANGLE_WAVE, SQUARE_WAVE, or HALF_SQUARE_WAVE
  void setWaveType(WaveformType _waveType) {
    waveType = _waveType;
  }

  void cycleWaveType() {
    if (waveType == SINE_WAVE) {
      waveType = TRIANGLE_WAVE;
    } else if (waveType == TRIANGLE_WAVE) {
      waveType = SQUARE_WAVE;
    } else if (waveType == SQUARE_WAVE) {
      waveType = HALF_SQUARE_WAVE;
    } else {
      waveType = SINE_WAVE;
    }
  }
    
  WaveformType getWaveType() {
    return waveType;
  }
    
  char* getWaveTypeName() {
    switch(waveType) {
      case SINE_WAVE: return "SIN";
      case TRIANGLE_WAVE: return "TRI";
      case SQUARE_WAVE: return "SQ1";
      case HALF_SQUARE_WAVE: return "SQ2";
    }
    return "???";
  }

  char* name() override {
    return "ad9833";
  }
  
private:
  AD9833 *ad9833;
  uint8_t FNCpin = 7;
  uint32_t referenceFrequency = 25000000UL;
  WaveformType waveType = SINE_WAVE;
  Registers freqReg = REG0;
  Registers phaseReg = SAME_AS_REG0;
  float phaseInDeg = 0;
};

#endif // GEN_9833_H
