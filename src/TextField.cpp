
#include <LiquidCrystal_I2C.h>
#include "TextField.h"

TextField::TextField(String name, bool isUsed, uint8_t x, uint8_t y, uint8_t length, String prefix, String suffix, uint8_t valueType) : _isUsed(isUsed), _x(x), _y(y), _length(length), _prefix(prefix), _suffix(suffix), _valueType(valueType)
{
  _inEditMode = false;
  _valuePrintable = false;
  _blinkValuePosition = -1;
  _blink = 1;
  switch (_valueType) {
    case 1:
    _numValueLength = 5;
    break;
    case 2:
    _numValueLength = 4;
    break;
    case 3:
    _numValueLength = 4;
    break;
    case 4:
    _numValueLength = 5;
    break;
    case 6:
    _numValueLength = 3;
    default:
    break;
  }
}

void TextField::drawPrefix(LiquidCrystal_I2C& lcd) {
  lcd.setCursor(_x, _y);
  lcd.print(_prefix);
}

void TextField::drawSuffix(LiquidCrystal_I2C& lcd) {

  lcd.setCursor(_x+_length-_suffix.length(), _y);
  lcd.print(_suffix);
  int8_t poss_ = _suffix.indexOf("-");
  if (poss_ != -1) {
    lcd.setCursor(_x+_length-_suffix.length()+poss_, _y);
    lcd.write(0);
  }
  int8_t poss = _suffix.indexOf(";");
  if (poss != -1) {
    lcd.setCursor(_x+_length-_suffix.length()+poss, _y);
    lcd.write(2);
  }
}

void TextField::draw(LiquidCrystal_I2C& lcd) {
  if (_isUsed == true) {
    if (_prefix.length() > 0) {
      drawPrefix(lcd);
    }
    if (_suffix.length() > 0) {
      drawSuffix(lcd);
    }
  }
}

int8_t TextField::setEditMode(LiquidCrystal_I2C& lcd) {
  uint8_t ind = 0;
  if (_suffix.endsWith(";") == 0) {
    ind = _length - 1;
  } else {
    ind = _length - 2;
  }
  lcd.setCursor(_x+ind, _y);
  lcd.write(1);       // Arrows
  _inEditMode = 1;
  _blinkValuePosition = 0;
  return _blinkValuePosition;
}

int8_t TextField::disableEditMode(LiquidCrystal_I2C& lcd) {
  _inEditMode = 0;
  _blinkValuePosition = -1;
  drawSuffix(lcd);
  return _blinkValuePosition;
}

void TextField::clearValue(LiquidCrystal_I2C &lcd) {
  lcd.setCursor(_x+_prefix.length(), _y);
  for (uint8_t i = 0; i < _length-_prefix.length()-_suffix.length(); i++) {
    lcd.print(' ');
  }
}

void TextField::printFrac(uint16_t divider, uint8_t fracLength, LiquidCrystal_I2C& lcd) {
  uint8_t valueLength = _length - _prefix.length() - _suffix.length();
  uint16_t card = _nvalue/divider;
  uint16_t frac = _nvalue%divider;
  uint8_t xoff = _prefix.length() + (valueLength - fracLength - 2);

  lcd.setCursor(_x+_prefix.length(), _y);
  for (uint8_t i = 0; i < valueLength; i++) {
    if (_blinkValuePosition != -1) {
      lcd.print('0');
    } else {
      lcd.print(' ');
    }
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
    if ((fracLength > 2) && (frac < 100) && (frac > 9)) {
      for (uint8_t i = 0; i < fracLength-2; i++) {
        lcd.print("0");
      }
    }
    lcd.print(frac);
  }
  if (_blinkValuePosition != -1) {     // Blinking
    lcd.setCursor(_x + _prefix.length() + valueLength - _blinkValuePosition - 1, _y);
    if (_blinkValuePosition >= fracLength) {
      lcd.setCursor(_x + _prefix.length() + valueLength - _blinkValuePosition - 2, _y);
    }
    if (_blink == 0) {
      lcd.print(' ');
    }
  }
}

void TextField::drawNumberValue(LiquidCrystal_I2C& lcd) {
  uint8_t valueLength = _length - _prefix.length() - _suffix.length();
  switch (_valueType) {
    case CURR_VALUE:
    printFrac(1000, 3, lcd);
    break;
    case VOLT_VALUE:
    printFrac(100, 2, lcd);
    break;
    case POWER_VALUE:
    printFrac(10, 1, lcd);
    break;
    case RESIST_VALUE:
    printFrac(10, 1, lcd);
    break;
    case CVOLT_VALUE:
    printFrac(100, 2, lcd);
    break;
    case TIME_VALUE:
    uint8_t hours = _nvalue/3600;
    uint8_t mins = (_nvalue%3600)/60;
    uint8_t secs = (_nvalue%3600)%60;
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

void TextField::drawStringValue(LiquidCrystal_I2C& lcd) {
  clearValue(lcd);
  lcd.setCursor(_x + _prefix.length(), _y);
  lcd.print(_svalue);
}

void TextField::drawValue(LiquidCrystal_I2C& lcd, int8_t pos) {
  //if ((_isUsed == true) && (_isValueNew == true)) {
  if ((_isUsed == true) && (_valuePrintable)) {
    if (_valueType != STRING_VALUE) {
      drawNumberValue(lcd);
    } else {
      drawStringValue(lcd);
    }
    //_isValueNew = false;
  } else if ((_isUsed == true) && (!_valuePrintable)) {
    clearValue(lcd);
  }
}

void TextField::setValue(String value) {
  _svalue = value;
  //_isValueNew = true;
}

void TextField::setValue(uint16_t value) {
  _nvalue = value;
  //_isValueNew = true;
}

int8_t TextField::shiftLeftValueBlinkPosition() {
  if (_blinkValuePosition != -1) {
    if (_blinkValuePosition < _numValueLength - 1) {
      _blinkValuePosition++;
    }
  }
  return _blinkValuePosition;
}

int8_t TextField::shiftRightValueBlinkPosition() {
  if (_blinkValuePosition != -1) {
    if (_blinkValuePosition > 0) {
      _blinkValuePosition--;
    }
  }
  return _blinkValuePosition;
}
