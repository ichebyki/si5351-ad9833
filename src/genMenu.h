#ifndef GEN_MENU_h
#define GEN_MENU_h

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>  // by Frank de Brabander

struct mitem {
    short ind;
    short val;
    const char* str;
} _mitem;

class genMenu {
   public:
    genMenu(LiquidCrystal_I2C* lcd, short count) {
        this->lcd = lcd;
        this->count = count;
        items = new mitem[count];
    }

    void show() {
        lcd->clear();
        lcd->setCursor(3, 0);
        lcd->print("SELECT MODE");

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

    short cycleCurrent(short dir) {
        if (dir > 0) {
            if (current == (count - 1)) {
                current = 0;
            } else {
                current++;
            }
        } else {
            if (current == 0) {
                current = count - 1;
            } else {
                current--;
            }
        }
        return current;
    }

    mitem* getCurrentItem() {
        return &items[current];
    }

    void setItem(short ind, short val, const char* str) {
        items[ind].ind = ind;
        items[ind].val = val;
        items[ind].str = str;
    }

   private:
    LiquidCrystal_I2C *lcd;
    short current = 0;
    short count;
    mitem* items;
};

#endif  // GEN_MENU_h
