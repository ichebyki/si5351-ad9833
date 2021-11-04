#ifndef GEN_X_h
#define GEN_X_h

#include <Arduino.h> 
#include <LiquidCrystal_I2C.h>    // by Frank de Brabander

class genBase {
public:
  genBase() { };

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

  void welcome() {
    lcd->clear();
    lcd->print(name());
    lcd->setCursor(0, 1);
    lcd->print("Version 1.0");
    delay(2000);
  }

  virtual void showFreq() {
    unsigned long frq = get_freq();
    unsigned int m = frq / 1000000;
    unsigned int k = (frq % 1000000) / 1000;
    unsigned int h = frq % 1000;
  
    lcd->setCursor(0, 0);
    char buffer[19] = "";
    if (m < 1) {
      sprintf(buffer, "%d.%03d%10s", k, h, "");
    } else {
      sprintf(buffer, "%d.%03d.%03d%7s", m, k, h, "");
    }
    lcd->print(buffer);
  }

  virtual void showInfo(bool showName) {
    if (showName) {
      lcd->setCursor(0, 1);
      lcd->print("                ");
      lcd->setCursor(0, 1);
      lcd->print(name());
    } else {
      unsigned long stp = get_fstep();
      lcd->setCursor(0, 1);
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

    void setEnabled(bool enable) { 
        enabled = enable; 
    }

    bool getEnabled() { 
        return enabled; 
    }

    void changeEnabled() {
        enabled = !enabled;
    }

    virtual void  init();
    virtual void  update();
    virtual void  updateFreq();
    virtual void updateEnabled();
    virtual const char *name();

protected:
    unsigned long freq = 100000;
    unsigned long fstep = 100000;
    bool enabled = true;
    LiquidCrystal_I2C *lcd;
};

#endif // GEN_X_h
