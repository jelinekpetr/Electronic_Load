
#include <LiquidCrystal_I2C.h>
#include "DisplayScreen.h"

DisplayScreen::DisplayScreen() {
  _refreshEnabled = true;
}

void DisplayScreen::addTextField(uint8_t index, String name, uint8_t x, uint8_t y, uint8_t length, String prefix, String suffix, uint8_t valueType) {

  textFields[index] = TextField(name, true, x, y, length, prefix, suffix, valueType);
}

void DisplayScreen::drawTextField(uint8_t index, LiquidCrystal_I2C& lcd) {
  if (textFields[index]._isUsed == true) {
    textFields[index].draw(lcd);
  }
}

void DisplayScreen::drawAll(LiquidCrystal_I2C& lcd) {
  if (_isActive == true) {
    lcd.clear();
    for (uint8_t i = 0; i < 10; i++) {
      if (textFields[i]._isUsed == true) {
        drawTextField(i, lcd);
      }
    }
  }
}

void DisplayScreen::setActive() {
  _isActive = true;
}

void DisplayScreen::setInactive() {
  _isActive = false;
}

void DisplayScreen::refresh(LiquidCrystal_I2C& lcd, int8_t pos) {
  if (_isActive == true) {
    for (uint8_t i = 0; i < 10; i++) {
      if (textFields[i]._isUsed == true) {
        if ((textFields[i]._inEditMode == true) && (pos != -1)) {
          textFields[i].drawValue(lcd, pos);
        } else {
          textFields[i].drawValue(lcd);
        }
      }
    }
  }
}
