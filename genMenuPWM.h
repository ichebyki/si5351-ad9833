#ifndef GEN_MENU_PWM_h
#define GEN_MENU_PWM_h

#include "utils.h"

#include "genMenu.h"

class genMenuPWM : public genMenu {
public:
    genMenuPWM(LiquidCrystal_I2C* lcd, short count, short pwm = 50)
        : genMenu(lcd, count) {
        this->lcd = lcd;
        this->count = count;
        items = new mitem[count];
        PWM = pwm;
    }

    void show() override {
        lcd->clear();
        lcd->setCursor(4, 0);
        lcd->print("PWM: ");
        lcd->print(PWM);
        lcd->print(" %");

        if (count > 2) {
            lcd->setCursor(0, 1);
            if (current == 0) {
                lcd->print(items[count - 1].str);
            } else {
                lcd->print(items[current - 1].str);
            }

            lcd->setCursor(12, 1);
            if (current == (count - 1)) {
                lcd->print(items[0].str);
            } else {
                lcd->print(items[current + 1].str);
            }

            lcd->setCursor(5, 1);
        } else if (count == 2) {
            if (current == 0) {
                lcd->setCursor(10, 1);
                lcd->print(items[count - 1].str);
                lcd->setCursor(4, 1);
            } else {
                lcd->setCursor(5, 1);
                lcd->print(items[current - 1].str);
                lcd->setCursor(9, 1);
            }
        } else {
            lcd->setCursor(5, 1);
        }
        lcd->print(">");
        lcd->print(items[current].str);
        lcd->print("<");
    }

    short changePWM(short dir) {
        if (dir > 0) {
            if (PWM < 99) {
                PWM += 1;
            }
        } else {
            if (PWM > 1) {
                PWM -= 1;
            }
        }
        return PWM;
    }

    short setPWM(short pwm) {
        if (pwm > 0 && pwm < 100) {
            PWM = pwm;
        }
        return PWM;
    }

    short getPWM() {
        return PWM;
    }

private:
    short PWM = 50;  // %% of the PWM
};

#endif  // GEN_MENU_PWM_h
