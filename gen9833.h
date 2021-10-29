#ifndef GEN_AD9833_H
#define GEN_AD9833_H

#include <SPI.h>
#define FSYNC_PIN 10 			 // Define FSYNC_PIN for fast digital writes
#define FNC_PIN FSYNC_PIN	 // Define FNC_PIN for fast digital writes
#include "genBase.h"

// ********** AD9833 **********
#define bMode 0x2
#define bDiv2 0x8
#define bOpbiten 0x20
#define bSleep12 0x40
#define bSleep1 0x80
#define bReset 0x100
#define bHLB 0x1000
#define bB28 0x2000
#define bCntrl_reg 0x0
#define bFreq_reg0 0x4000
#define bFreq_reg1 0x8000
#define bPhase_reg 0xC000
enum eWaveForm {wfSin, wfTri, wfSqr, wfSqr2};

class gen9833: public genBase {
public:
  gen9833(short _FNCpin = FSYNC_PIN, uint32_t _referenceFrequency = 25000000UL) {
    FNCpin = _FNCpin;
    referenceFrequency = _referenceFrequency;
  }

  void init() {
    WriteAD9833(bCntrl_reg | bReset | bB28);
    SetFrequency(freq); // Установим частоту
    SetForm(waveType);
    SetPhase(0); // Сдвиг по фазе 0
    WriteAD9833(bCntrl_reg | bB28); // Снимаем Reset
  }

  void changeEnabled() override {
    genBase::changeEnabled();
    if (enabled) {
      WriteAD9833(bCntrl_reg | bB28);
    } else {
      WriteAD9833(bCntrl_reg | bReset | bB28);
    }
  }

  void update() {
    WriteAD9833(bCntrl_reg | bReset | bB28);
    SetFrequency(freq);
    SetForm(waveType);

    SetPhase(0); // Сдвиг по фазе 0
    WriteAD9833(bCntrl_reg | bB28); // Снимаем Reset
  }

  void change_fstep(short dir = 1) override {
    genBase::change_fstep(dir);
    if (fstep > 1000000) {
      if (dir > 0) {
        fstep = 1;
      } else {
        fstep = 1000000;
      }
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
  void setWaveType(eWaveForm _waveType) {
    S("--- setWaveType ---");
    waveType = _waveType;
  }

  void cycleWaveType() {
    if (waveType == wfSin) {
      waveType = wfTri;
    } else if (waveType == wfTri) {
      waveType = wfSqr;
    } else if (waveType == wfSqr) {
      waveType = wfSqr2;
    } else {
      waveType = wfSin;
    }
    S("--- cycleWaveType ---");
  }
    
  eWaveForm getWaveType() {
    return waveType;
  }
    
  const char* getWaveTypeName() {
    switch(waveType) {
      case wfSin: return "SIN";
      case wfTri: return "TRI";
      case wfSqr: return "SQ1";
      case wfSqr2: return "SQ2";
    }
    return "???";
  }

  const char* name() override {
    return "ad9833";
  }
  
private:
  uint8_t FNCpin = FSYNC_PIN;
  unsigned long referenceFrequency = 25000000UL;
  eWaveForm waveType = wfSin;
  int freqReg = bFreq_reg0;
  int phaseReg = bPhase_reg;
  int phaseInDeg = 0;

private:
  // ******************** Установить частоту ********************
  void SetFrequency(unsigned long val) {
    freq = val;
    unsigned long FreqData = round((float) val * 10.73741 + 0.5);
    WriteAD9833((FreqData & 0x3FFF) | bFreq_reg0);
    WriteAD9833((FreqData >> 14) | bFreq_reg0);
  }

  // ******************** Установить фазу ********************
  void SetPhase(int val) {
    phaseInDeg = val;
    unsigned long PhaseData = round (float(val) * 11.37777 + 0.5);
    WriteAD9833(PhaseData | bPhase_reg);
  }

  // ******************** Установить форму ********************
  void SetForm(eWaveForm val) {
    waveType = val;
    int16_t CntrlData = 0;
    switch (val) {
      case wfSin:
          CntrlData = 0;
          break;
      case wfTri:
          CntrlData = bMode;
          break;
      case wfSqr:
          CntrlData = bOpbiten | bDiv2 | bSleep12;
          break;
      case wfSqr2:
          CntrlData = bOpbiten | bSleep12;
          break;
    }
    WriteAD9833(CntrlData | bCntrl_reg | bB28);
    S("--- SetForm ---");
    S16("set wave = ", CntrlData);
    S16(" / 0x", CntrlData | bCntrl_reg | bB28);
  }

  // ******************** Передача 16-битного слова в AD9833 ********************
  void WriteAD9833(uint16_t Data) {
    SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV8, MSBFIRST, SPI_MODE2));
    digitalWrite(FNCpin, LOW);
    delayMicroseconds(1);
    SPI.transfer16(Data);
    digitalWrite(FNCpin, HIGH);
    SPI.endTransaction();
  }
};

#endif // GEN_9833_H
