
#include <LiquidCrystal_I2C.h>
#include "DisplayScreen.h"

DisplayScreen::DisplayScreen()
{
}

void DisplayScreen::addTextField(uint8_t index, String name, uint8_t x, uint8_t y, uint8_t length, String prefix, String suffix) {

  textFields[index] = TextField(name, x, y, length, prefix, suffix);
}

void DisplayScreen::drawTextField(uint8_t index, LiquidCrystal_I2C& lcd) {

  textFields[index].draw(lcd);
}

void DisplayScreen::drawAll(LiquidCrystal_I2C& lcd, uint8_t countField) {
  lcd.clear();
  for (uint8_t i = 0; i < countField; i++) {
    drawTextField(i, lcd);
  }
}
