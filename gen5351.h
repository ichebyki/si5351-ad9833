#ifndef GEN_SI5351_H
#define GEN_SI5351_H

#include <si5351.h>               // Etherkit https://github.com/etherkit/Si5351Arduino
#include "genBase.h"

class gen5351: public genBase {
public:
  gen5351(uint8_t _i2c_addr = SI5351_BUS_BASE_ADDR) {
    i2c_addr = _i2c_addr;
  }

  void init() {
    si5351 = new Si5351(i2c_addr);
    
    si5351->init(SI5351_CRYSTAL_LOAD_8PF, 0, cal);
    si5351->output_enable(SI5351_CLK0, 1);                  //1 - Enable / 0 - Disable CLK
    si5351->output_enable(SI5351_CLK1, 0);
    si5351->output_enable(SI5351_CLK2, 0);
    si5351->drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);  //Output current 2MA, 4MA, 6MA or 8MA

    update();
  }

  void update() {
    updateFreq();
  }

  void updateFreq() {
    si5351->set_freq_manual((freq + (interfreq * 1000ULL)) * 100ULL,
                            pll_freq,
                            SI5351_CLK0);
  }

  void showFreq(LiquidCrystal_I2C *lcd) override {
    genBase::showFreq(lcd);
  }
  
  void showInfo(LiquidCrystal_I2C *lcd, bool showName) override {
    genBase::showInfo(lcd, showName);
  }
  
  void change_freq(short dir) override {
    genBase::change_freq(dir);
    
    if (freq < 10000) {
      freq = 10000;
    } else if (freq > 225000000) {
      freq = 225000000;
    }
  }

  const char* name() override {
    return "si5331";
  }
  
private:
  Si5351 *si5351;
  uint8_t i2c_addr = SI5351_BUS_BASE_ADDR;
  long interfreq = 0;   // Enter your IF frequency, ex: 455 = 455kHz, 10700 = 10.7MHz, 0 = to direct convert receiver or RF generator, + will add and - will subtract IF offfset.
  long cal = 33000;     // Si5351 calibration factor, adjust to get exatcly 10MHz. Increasing this value will decreases the frequency and vice versa.
  unsigned long long pll_freq = 90000000000ULL;
};


#endif // GEN_5351_H
