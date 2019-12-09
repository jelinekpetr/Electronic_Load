
#include <Arduino.h>
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#include "DisplayScreen.h"

#define VERSION "0.0.1"

byte omega[] = {B01110,B10001,B10001,B10001,B01010,B01010,B11011,B00000};
byte arrows[] = {B00100,B01110,B11111,B00000,B11111,B01110,B00100, B00000};
byte block[] = {B01110,B01110,B01110,B01110,B01110,B01110,B01110,B01110};

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
DisplayScreen mainScreen = DisplayScreen();

unsigned long startTime;
uint16_t elapsedTime = 0;
uint32_t energy = 0; // v mAs
uint16_t current = 0;

void initLCD() {
  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, omega);
  lcd.createChar(1, arrows);
  lcd.createChar(2, block);

}

void showStartupScreen() {
  DisplayScreen startupScreen = DisplayScreen();

  startupScreen.addTextField(0, "Label", 0, 0, 20, "", "");
  startupScreen.addTextField(1, "Firmware", 0, 1, 20, "Firmware:", "");

  startupScreen.drawAll(lcd, 2);

  startupScreen.textFields[0].updateValue("  Electronic Load  ", lcd);
  startupScreen.textFields[1].updateValue(VERSION, lcd);
}

void showSetupScreen() {


}

void setup() {

  initLCD();

  showStartupScreen();
  delay(3000);

  // main screen
  mainScreen.addTextField(0, "Cur", 0, 0, 11, "I:", "A ;");
  mainScreen.addTextField(1, "Vol", 0, 1, 11, "U:", "V ;");
  mainScreen.addTextField(2, "Pow", 0, 2, 11, "P:", "W ;");
  mainScreen.addTextField(3, "Res", 0, 3, 11, "R:", "- ;");
  mainScreen.addTextField(4, "Tim", 11, 0, 9, "", "");
  mainScreen.addTextField(5, "Ah", 11, 1, 9, "", "Ah");
  mainScreen.addTextField(6, "Vco", 11, 2, 9, "Vc:", "V ");
  mainScreen.addTextField(7, "Mode", 11, 3, 3, "", ";");
  mainScreen.addTextField(8, "XXX", 14, 3, 3, "", ";");
  mainScreen.addTextField(9, "Load", 17, 3, 3, "", "");

  mainScreen.drawAll(lcd);

  startTime = 0;


}

void loop() {

  /*
  mainScreen.textFields[1].setEditable(lcd);
  delay(5000);
  mainScreen.textFields[1].disableEditable(lcd);
  delay(5000);
  mainScreen.textFields[0].setEditable(lcd);
  delay(5000);
  mainScreen.textFields[0].disableEditable(lcd);
  */


  if ((millis() - startTime) > 1000) {
    startTime = millis();
    energy = mainScreen.textFields[0]._value + energy;
    current = uint16_t (random(8950,10568));

    mainScreen.textFields[0].updateValue(current, CURR_VALUE, lcd);
    mainScreen.textFields[1].updateValue(2125, VOLT_VALUE, lcd);
    mainScreen.textFields[2].updateValue(uint16_t (mainScreen.textFields[1]._value*mainScreen.textFields[0]._value/100000), POWER_VALUE, lcd);
    mainScreen.textFields[3].updateValue(mainScreen.textFields[1]._value*100/mainScreen.textFields[0]._value, RESIST_VALUE, lcd);
    mainScreen.textFields[4].updateValue(uint16_t (millis()/1000), TIME_VALUE, lcd);
    mainScreen.textFields[5].updateValue(uint16_t (energy/3600), CURR_VALUE, lcd);
    mainScreen.textFields[6].updateValue(0, VOLT_VALUE, lcd);
    mainScreen.textFields[7].updateValue("CC", lcd);
    mainScreen.textFields[9].updateValue("On", lcd);


  }



}
