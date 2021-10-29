#ifndef GEN_X_h
#define GEN_X_h

#include <Arduino.h> 
#include <LiquidCrystal_I2C.h>    // by Frank de Brabander

class genBase {
public:
  genBase(uint8_t addr = 0) {};

  unsigned long get_freq() {
    return freq;
  }

  unsigned long get_fstep() {
    return fstep;
  }

  virtual void change_fstep(short dir = 1) {
    if (dir > 0) {
      if (fstep == 10000000) {
          fstep = 1;
      } else if (fstep <= 1000000) {
          fstep *= 10;
      } else {
          fstep = 10000000;
      }
    } else if (dir < 0) {
      if (fstep == 1) {
          fstep = 10000000;
      } else if (fstep > 1) {
          fstep /= 10;
      } else {
          fstep = 1;
      }
    }
  }
  
  virtual void change_freq(short dir) {
    if (dir == 1) {
      freq = freq + fstep;
    } else if (dir == -1) {
      if (freq > fstep) {
        freq = freq - fstep;
      }
    }
  }
  
  bool check_and_update() {
    if (freqPrev != freq) {
      update();
      freqPrev = freq;
      return true;
    }
    return false;
  }

  void welcome(LiquidCrystal_I2C *lcd) {
    lcd->clear();
    lcd->print(name());
    lcd->print(" Generator");
    lcd->setCursor(0, 1);
    lcd->print("Version 1.0");
    delay(2000);
    lcd->clear();
  }

  virtual void showFreq(LiquidCrystal_I2C *lcd) {
    unsigned long frq = get_freq();
    unsigned int m = frq / 1000000;
    unsigned int k = (frq % 1000000) / 1000;
    unsigned int h = frq % 1000;
  
    lcd->setCursor(0, 0);
    char buffer[19] = "";
    if (m < 1) {
      sprintf(buffer, "%d.%003d%10s", k, h, "");
    } else {
      sprintf(buffer, "%d.%003d.%003d%7s", m, k, h, "");
    }
    lcd->print(buffer);
  }

  virtual void showInfo(LiquidCrystal_I2C *lcd, bool showName) {
    lcd->setCursor(0, 1);
    if (showName) {
      lcd->print("generator ");
      lcd->print(name());
    } else {
      unsigned long stp = get_fstep();
      lcd->print("step ");
      if (stp < 1000) {
        lcd->print(stp);
        lcd->print(" Hz"); 
      } else if (stp < 1000000) {
        lcd->print(stp / 1000);
        lcd->print(" kHz"); 
      } else {
        lcd->print(stp / 1000000);
        lcd->print(" MHz"); 
      }
      lcd->print("       ");
    }
  }

  virtual void  init();
  virtual void  update();
  virtual char* name();
  virtual void  changeEnabled() { enabled = !enabled; }
  virtual bool  getEnabled() { return enabled; }

protected:
  unsigned long freq = 100000;
  unsigned long freqPrev = 100000;
  unsigned long fstep = 100000;
  bool enabled = true;
};

#endif // GEN_X_h
