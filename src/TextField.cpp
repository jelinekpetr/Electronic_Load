
#include <LiquidCrystal_I2C.h>
#include "TextField.h"

TextField::TextField(String _name, uint8_t x, uint8_t y, uint8_t length, String prefix, String suffix) : _x(x), _y(y), _length(length), _prefix(prefix), _suffix(suffix)
{
}

/*
uint8_t TextField::getPrefixLength() {
  return _prefix.length();
}
*/
/*
uint8_t TextField::getSuffixLength() {
  return _suffix.length();
}
*/
void TextField::drawPrefix(LiquidCrystal_I2C& lcd) {
  lcd.setCursor(_x, _y);
  lcd.print(_prefix);
}

void TextField::drawSuffix(LiquidCrystal_I2C& lcd, int8_t special) {

  lcd.setCursor(_x+_length-_suffix.length(), _y);
  lcd.print(_suffix);

  if (special != -1) {
    lcd.setCursor(_x+_length-_suffix.length()+_suffix.indexOf("-"), _y);
    lcd.write(special);
  }

  int8_t poss = _suffix.indexOf(";");
  if (poss != -1) {
    lcd.setCursor(_x+_length-_suffix.length()+poss, _y);
    lcd.write(2);
  }

}

void TextField::draw(LiquidCrystal_I2C& lcd, int8_t special) {
  if (_prefix.length() > 0) {
    drawPrefix(lcd);
  }
  if (_suffix.length() > 0) {
    if (_prefix.compareTo("R:") == 0) {
      drawSuffix(lcd, 0);
    } else {
      drawSuffix(lcd);
    }
  }
}

void TextField::setEditable(LiquidCrystal_I2C& lcd) {
  _isEditable = 1;
  uint8_t ind = 0;
  if (_suffix.endsWith(";") == 0) {
    ind = _length - 1;
  } else {
    ind = _length - 2;
  }
  lcd.setCursor(_x+ind, _y);
  lcd.write(1);
  lcd.setCursor(_x + _length - _suffix.length() - 1, _y);
  lcd.blink();

}

void TextField::disableEditable(LiquidCrystal_I2C& lcd) {
  _isEditable = 0;
  drawSuffix(lcd);
  lcd.noBlink();

}

void TextField::printFrac(uint16_t value, uint16_t divider, uint8_t valueLength, uint8_t fracLength, LiquidCrystal_I2C& lcd) {
  uint16_t card = value/divider;
  uint16_t frac = value%divider;
  uint8_t xoff = _prefix.length() + (valueLength - fracLength - 2);

  lcd.setCursor(_x+_prefix.length(), _y);
  for (uint8_t i = 0; i < valueLength; i++) {
    lcd.print(" ");
  }

  if (card > 9) {xoff--;}
  if (card > 99) {xoff--;}
  if (card > 999) {xoff--;}
  if (card > 9999) {xoff--;}
  if (fracLength == 0) {xoff++;}

  lcd.setCursor(_x + xoff, _y);
  lcd.print(card);
  if (fracLength > 0) {
    lcd.print(".");
    if (fracLength > 1 && frac < 10) {
      for (uint8_t i = 0; i < fracLength-1; i++) {
        lcd.print("0");
      }
    }
    if (fracLength > 2 && frac < 100 && frac > 9) {
      for (uint8_t i = 0; i < fracLength-2; i++) {
        lcd.print("0");
      }
    }
    lcd.print(frac);
  }
}

void TextField::updateValue(uint16_t value, uint8_t type, LiquidCrystal_I2C& lcd) {
  _value = value;
  uint8_t valueLength = _length - _prefix.length() - _suffix.length();

  switch (type) {
    case CURR_VALUE:
    printFrac(value, 1000, valueLength, 3, lcd);
    break;
    case VOLT_VALUE:
    printFrac(value, 100, valueLength, 2, lcd);
    break;
    case POWER_VALUE:
    printFrac(value, 10, valueLength, 1, lcd);
    break;
    case RESIST_VALUE:
    printFrac(value, 10, valueLength, 1, lcd);
    break;
    case TIME_VALUE:
    uint8_t hours = value/3600;
    uint8_t mins = (value%3600)/60;
    uint8_t secs = (value%3600)%60;
    uint8_t xoff = valueLength-7;
    if (hours > 9) {xoff--;}
    if (hours > 99) {xoff--;}
    lcd.setCursor(_x + xoff, _y);
    lcd.print(hours);

    lcd.print(":");
    if (mins < 10) {lcd.print("0");}
    lcd.print(mins);

    lcd.print(":");
    if (secs < 10) {lcd.print("0");}
    lcd.print(secs);
    break;
  }
}

void TextField::updateValue(String value, LiquidCrystal_I2C& lcd) {
  uint8_t valueLength = _length - _prefix.length() - _suffix.length();
  lcd.setCursor(_x + _prefix.length(), _y);
  for (uint8_t i = 0; i < valueLength; i++) {
    lcd.print(" ");
  }
  lcd.setCursor(_x + _prefix.length(), _y);
  lcd.print(value);
}
