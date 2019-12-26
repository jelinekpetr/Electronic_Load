
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>              // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <Keypad.h>            // Keypad
#include "DisplayScreen.h"
#include "KeypadBuffer.h"

#define VERSION "0.1"
#define CC_MODE 0
#define CV_MODE 1
#define CP_MODE 2
#define CR_MODE 3
#define SWITCH_ON_1 22  ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define SWITCH_ON_10 23 ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/*******************************************************************************/
/*                               Devices/objects                               */
/*******************************************************************************/
/*
*|   LCD
*/
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

/*
*|   Keypad
*/
const byte ROWS = 5; //five rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
    {'X', 'Y', '#', '*'},
    {'1', '2', '3', '^'},
    {'4', '5', '6', 'v'},
    {'7', '8', '9', 'Q'},
    {'<', '0', '>', 'E'}};
byte rowPins[ROWS] = {2, 3, 4, 5, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 9, 8, 7};   //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad myKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

/*********************************************/
/*             Keypad buffer                 */
/*********************************************/
KeypadBuffer keypadBuffer = KeypadBuffer();

/*********************************************/
/*          Main screen on LCD               */
/*********************************************/
DisplayScreen mainScreen = DisplayScreen();

/*******************************************************************************/
/*                               Global variables                              */
/*******************************************************************************/

/*********************************************/
/*        Special characters on LCD          */
/*********************************************/
byte omega[] = {B01110, B10001, B10001, B10001, B01010, B01010, B11011, B00000};
byte arrows[] = {B00100, B01110, B11111, B00000, B11111, B01110, B00100, B00000};
byte block[] = {B01110, B01110, B01110, B01110, B01110, B01110, B01110, B01110};

/*********************************************/
/*             Status register               */
/*********************************************/
/* 0 - On/Off  (0 = Off, 1=On)               */
/* 1 - Device mode (CC, CU, CP, CR)          */
/* 2 - Operation/Setup/SetupVco (0/1/2)      */
/* 3 - Blink                                 */
/*********************************************/
uint8_t statusRegister[4];

/*********************************************/
/*            Measured values                */
/*********************************************/
/* 0 - Current in mA                         */
/* 1 - Voltage in tens of mV                 */
/* 2 - Power in tenths of W                  */
/* 3 - Resistivity in tenths of Ohm          */
/* 4 - Time in seconds                       */
/* 5 - Capacity in mAh                       */
/*                                           */
/*********************************************/
uint16_t measuredValues[7];

/*********************************************/
/*            Setup values                   */
/*********************************************/
/* 0 - CC - value                            */
/* 1 - CV - value                            */
/* 2 - CP - value                            */
/* 3 - CR - value                            */
/* 4 - unused                                */
/* 5 - unused                                */
/* 6 - Vco                                   */
/*********************************************/
uint16_t setupValues[7];

unsigned long startTimeMeasure, startTimeRefresh, startTimeKeyboard, startTime, startTimeBlink;
uint16_t measurePeriod, refreshPeriod, keyboardPeriod, startTimeKeyboardBuffer, keyboardBufferPeriod, blinkPeriod;
uint32_t elapsedTime = 0;
uint32_t mAs = 0;
bool cleared = false;
int8_t setupPos = -1;

/*
uint32_t energy = 0; // v mAs
uint16_t current = 0;
uint16_t voltage = 0;
uint16_t power = 0;
uint16_t resistivity = 0;
*/

/*******************************************************************************/
/*                               Functions                                     */
/*******************************************************************************/

/*********************************************/
/*         Initialization of LCD HW          */
/*********************************************/
void initLCD()
{
  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, omega);
  lcd.createChar(1, arrows);
  lcd.createChar(2, block);
}

void showStartupScreen()
{
  DisplayScreen startupScreen = DisplayScreen();
  startupScreen.addTextField(0, "Label", 0, 0, 20, "", "", STRING_VALUE);
  startupScreen.addTextField(1, "Firmware", 0, 1, 20, "FW: ", "", STRING_VALUE);
  startupScreen.setActive();
  startupScreen.drawAll(lcd);
  startupScreen.textFields[0].setValue("  Electronic Load  ");
  startupScreen.textFields[1].setValue(VERSION);
  startupScreen.refresh(lcd);
}

void showSetupScreen()
{
  DisplayScreen setupScreen = DisplayScreen();
  setupScreen.addTextField(0, "MaxVolt", 0, 0, 14, "Umax:", "V ;", VOLT_VALUE);
  setupScreen.addTextField(1, "MaxCurr", 0, 1, 14, "Imax:", "A ;", CURR_VALUE);
  setupScreen.addTextField(2, "MaxPow", 0, 2, 14, "Pmax:", "W ;", POWER_VALUE);
  setupScreen.drawAll(lcd);
}

/*********************************************/
/*    Toggle blink in statusRegister         */
/*********************************************/
void blinkB()
{
  if (statusRegister[3] == 1)
  {
    statusRegister[3] = 0;
  }
  else if (statusRegister[3] == 0)
  {
    statusRegister[3] = 1;
  }
  // Jak vybrat aktivni obrazovku?
  for (uint8_t i = 0; i <= 9; i++)
  {
    if ((mainScreen.textFields[i]._isUsed) && (mainScreen.textFields[i]._inEditMode))
    {
      mainScreen.textFields[i]._blink = statusRegister[3];
    }
  }
}

/*********************************************/
/*            Keypad Event listener          */
/*********************************************/
void keypadEvent(KeypadEvent key)
{
  switch (myKeypad.getState())
  {
  case PRESSED:
    break;
  case RELEASED:
    break;
  case HOLD:
    if (key == 'E')
    {
      keypadBuffer.pushChar('C');
    }
    if (key == 'X')
    {
      keypadBuffer.pushChar('O');
    }
    if (key == 'Y')
    {
      keypadBuffer.pushChar('P');
    }
    break;
  case IDLE:
    break;
  }
}

/*********************************************/
/*            Set load On                    */
/*********************************************/
void loadOn()
{
  // send preset value to DAC1

  // send preset value to DAC10

  digitalWrite(SWITCH_ON_1, LOW);
  digitalWrite(SWITCH_ON_10, LOW);
  cleared = false; // Values populated again
  mAs = 0;
  startTime = millis();
  elapsedTime = 0;
  statusRegister[0] = 1;
}

/*********************************************/
/*            Set load Off                   */
/*********************************************/
void loadOff()
{
  // send 0 to DAC1

  // send 0 to DAC10

  digitalWrite(SWITCH_ON_1, HIGH);
  digitalWrite(SWITCH_ON_10, HIGH);
  statusRegister[0] = 0;
}

/*********************************************/
/*            Toggle On/Off                  */
/*********************************************/
void toggleLoad()
{
  if (statusRegister[0] == 1)
  {
    loadOff();
  }
  else
  {
    loadOn();
  }
  cleared = false;
}

/*********************************************/
/*             Clearing values               */
/*********************************************/
void clearValues()
{
  if (!cleared)
  { // clearing values
    for (uint8_t i = 0; i < 6; i++)
    {
      mainScreen.textFields[i].clearValue(lcd);
    }
  }
  cleared = true;
}

/*********************************************/
/*      Refresh values in text fields        */
/*********************************************/
void setNewValues()
{
  String mode, state;
  switch (statusRegister[0])
  {
  case 0:
    state = "OFF";
    break;
  case 1:
    state = "ON";
    break;
  }
  switch (statusRegister[1])
  {
  case 0:
    mode = "CC";
    break;
  case 1:
    mode = "CV";
    break;
  case 2:
    mode = "CP";
    break;
  case 3:
    mode = "CR";
    break;
  }
  for (uint8_t i = 0; i < 6; i++)
  {
    mainScreen.textFields[i]._valuePrintable = false;
  }
  uint8_t oState = ((statusRegister[2] << 1) | statusRegister[0]);
  switch (oState)
  {
  case 0: // OFF + Operation
    if ((mainScreen.textFields[statusRegister[1]]._inEditMode) || (mainScreen.textFields[6]._inEditMode))
    {
      setupPos = mainScreen.textFields[statusRegister[1]].disableEditMode(lcd);
      setupPos = mainScreen.textFields[6].disableEditMode(lcd);
    }
    //clearValues();
    mainScreen.textFields[statusRegister[1]]._valuePrintable = true;
    mainScreen.textFields[statusRegister[1]].setValue(setupValues[statusRegister[1]]);
    mainScreen.textFields[6]._valuePrintable = true;
    mainScreen.textFields[6].setValue(setupValues[6]);
    break;
  case 1: // ON + Operation
    if (mainScreen.textFields[statusRegister[1]]._inEditMode)
    {
      setupPos = mainScreen.textFields[statusRegister[1]].disableEditMode(lcd);
      setupPos = mainScreen.textFields[6].disableEditMode(lcd);
    }
    //clearValues();
    for (uint8_t i = 0; i < 7; i++)
    {
      mainScreen.textFields[i]._valuePrintable = true;
      mainScreen.textFields[i].setValue(measuredValues[i]);
    }
    break;
  case 2: // OFF + Setup
    //clearValues();
    mainScreen.textFields[statusRegister[1]]._valuePrintable = true;
    mainScreen.textFields[statusRegister[1]].setValue(setupValues[statusRegister[1]]);
    if (mainScreen.textFields[6]._inEditMode)
    {
      setupPos = mainScreen.textFields[6].disableEditMode(lcd);
    }
    if (!mainScreen.textFields[statusRegister[1]]._inEditMode)
    {
      setupPos = mainScreen.textFields[statusRegister[1]].setEditMode(lcd);
      //mainScreen._refreshEnabled = false;
    }
    break;
  case 4: // OFF + Setup Vco
    if (mainScreen.textFields[statusRegister[1]]._inEditMode)
    {
      setupPos = mainScreen.textFields[statusRegister[1]].disableEditMode(lcd);
      mainScreen.textFields[statusRegister[1]]._valuePrintable = true;
      mainScreen.textFields[statusRegister[1]].setValue(setupValues[statusRegister[1]]);
      setupPos = mainScreen.textFields[6].setEditMode(lcd);
    }
    mainScreen.textFields[6]._valuePrintable = true;
    mainScreen.textFields[6].setValue(setupValues[6]);
    break;
  default:
    break;
  }

  // MOde and state
  mainScreen.textFields[7]._valuePrintable = true;
  mainScreen.textFields[7].setValue(mode);
  mainScreen.textFields[8]._valuePrintable = true;
  mainScreen.textFields[8].setValue(String(oState));
  mainScreen.textFields[9]._valuePrintable = true;
  mainScreen.textFields[9].setValue(state);
}

/*********************************************/
/*            Refresh screen                 */
/*********************************************/
void refreshScreen()
{
  setNewValues();
  mainScreen.refresh(lcd);
}

/*********************************************/
/*         Fill keyboard buffer             */
/*********************************************/
void fillKeyboardBuffer()
{
  char key = myKeypad.getKey();
  if (key)
  {
    digitalWrite(33, HIGH);
    keypadBuffer.pushChar(key);
    //delay(5);
    digitalWrite(33, LOW);
  }
}

/*********************************************/
/*               Measuring                   */
/*********************************************/
void measureValues()
{
  //uint16_t measuredValues[7];
  /************************************/
  /* 0 - Current in mA                */
  /* 1 - Voltage in tens of mV        */
  /* 2 - Power in tenths of W         */
  /* 3 - Resistivity in tenths of Ohm */
  /* 4 - Time in seconds              */
  /* 5 - Capacity in mAh              */
  /************************************/
  if (statusRegister[0] != 0)
  { // Load on
    elapsedTime = millis() - startTime;
    measuredValues[0] = uint16_t(random(925, 968)); // current
    measuredValues[1] = 1225;
    measuredValues[2] = uint16_t(measuredValues[0] * measuredValues[1] / 10000);
    measuredValues[3] = measuredValues[1] * 100 / measuredValues[0];
    measuredValues[4] = uint16_t(elapsedTime / 1000);
    mAs += measuredValues[0] * measurePeriod / 1000;
    measuredValues[5] = uint16_t(mAs / 3600);
  }
  else
  {
    measuredValues[0] = 0;
    measuredValues[1] = 1225; // voltage
    measuredValues[2] = 0;
    measuredValues[3] = -1; // !!!!!!!!!!!
    measuredValues[4] = 0;
    measuredValues[5] = 0;
  }
}

/*********************************************/
/*        Switch mode (left arrow)           */
/*********************************************/
void toggleModeLeft()
{
  if (statusRegister[1] > 0)
  {
    statusRegister[1] -= 1;
  }
  else if (statusRegister[1] == 0)
  {
    statusRegister[1] = 3;
  }
  cleared = false;
  if (!mainScreen._refreshEnabled)
  {
    mainScreen._refreshEnabled = true;
  }
}

/*********************************************/
/*        Switch mode (right arrow)          */
/*********************************************/
void toggleModeRight()
{
  if (statusRegister[1] < 3)
  {
    statusRegister[1] += 1;
  }
  else if (statusRegister[1] == 3)
  {
    statusRegister[1] = 0;
  }
  cleared = false;
  if (!mainScreen._refreshEnabled)
  {
    mainScreen._refreshEnabled = true;
  }
}

/*********************************************/
/*        calculate inc/dec                  */
/*********************************************/
uint16_t incDecValue()
{
  uint16_t val = 1;
  for (uint8_t i = 1; i <= setupPos; i++)
  {
    val *= 10;
  }
  return val;
}

/*********************************************/
/*           Read from EEPROM                */
/*********************************************/
void readFromEEPROM(int8_t baseAddress) // base address: 0, 1, 2...
{
  // EEPROM read preset values from base address
  for (uint16_t i = 0; i < 7; i++)
  {
    setupValues[i] = (uint16_t (EEPROM.read(2 * (i + baseAddress * 7) + 1)) << 8) | EEPROM.read(2 * (i + baseAddress * 7));
  }
}

/*********************************************/
/*           Write to EEPROM                 */
/*********************************************/
void writeToEEPROM(uint8_t baseAddress)
{
  // EEPROM write to base address
  for (uint8_t i = 0; i < 7; i++)
  {
    EEPROM.write(2 * (i + baseAddress * 7) + 1, uint8_t((setupValues[i] >> 8) & (0xFF)));
    EEPROM.write(2 * (i + baseAddress * 7), uint8_t(setupValues[i] & (0xFF)));
  }
  EEPROM.write(2 * (4 + baseAddress * 7), 1);
}

void eraseEEPROM()
{
  for (uint8_t i = 0; i < 24; i++)
  {
    EEPROM.write(i, 255);
  }
}

/*********************************************/
/*           Check keyboard buffer           */
/*********************************************/
void checkKeyboardBuffer()
{
  char k = keypadBuffer.getChar();
  if (k != 255)
  {
    //mainScreen.textFields[8].setValue(String (k));   // to display
    switch (k)
    {
    case '*':
      if (statusRegister[2] == 0)
      {
        toggleLoad();
      }
      break;
    case 'C':
      if (statusRegister[2] == 0)
      {
        statusRegister[2] = 1; // Enter setup mode
      }
      else if ((statusRegister[2] == 1) || (statusRegister[2] == 2))
      {
        statusRegister[2] = 0; // Enter Operation mode
      }
      cleared = false;
      break;
    case '>':
      if ((statusRegister[0] == 0) && (statusRegister[2] == 0))
      { // only if Off and no setup
        toggleModeRight();
      }
      if ((statusRegister[2] == 1) || (statusRegister[2] == 2))
      { // Setup
        // Jak vybrat aktivni obrazovku?
        for (uint8_t i = 0; i <= 9; i++)
        {
          if ((mainScreen.textFields[i]._isUsed) && (mainScreen.textFields[i]._inEditMode))
          {
            setupPos = mainScreen.textFields[i].shiftRightValueBlinkPosition();
          }
        }
      }
      break;
    case '<':
      if ((statusRegister[0] == 0) && (statusRegister[2] == 0))
      { // only if Off and no setup
        toggleModeLeft();
      }
      if ((statusRegister[2] == 1) || (statusRegister[2] == 2))
      { // Setup
        // Jak vybrat aktivni obrazovku?
        for (uint8_t i = 0; i <= 9; i++)
        {
          if ((mainScreen.textFields[i]._isUsed) && (mainScreen.textFields[i]._inEditMode))
          {
            setupPos = mainScreen.textFields[i].shiftLeftValueBlinkPosition();
          }
        }
      }
      break;
    case 'E':
      if (statusRegister[2] == 1)
      { // Setup
        statusRegister[2] = 2;
      }
      else if (statusRegister[2] == 2)
      {
        statusRegister[2] = 1;
      }
      //cleared = false;
      break;
    case '^':
      if ((statusRegister[2] == 1) || (statusRegister[2] == 2))
      {
        if (mainScreen.textFields[6]._inEditMode)
        {
          setupValues[6] += incDecValue();
        }
        else
        {
          setupValues[statusRegister[1]] += incDecValue();
        }
      }
      break;
    case 'v':
      if ((statusRegister[2] == 1) || (statusRegister[2] == 2))
      {
        if (mainScreen.textFields[6]._inEditMode)
        {
          setupValues[6] -= incDecValue();
        }
        else
        {
          if (setupValues[statusRegister[1]] > 0)
          {
            setupValues[statusRegister[1]] -= incDecValue();
          }
        }
      }
      break;
    case 'O':
      if ((statusRegister[2] == 1) || (statusRegister[2] == 2))
      {
        writeToEEPROM(0);
      }
      break;
    case '1':
      eraseEEPROM();
      break;
    default:
      break;
    }
  }
}

void checkDeviceStatus()
{
}

/*******************************************************************************/
/*                                   setup()                                   */
/*******************************************************************************/
void setup()
{

  //uint8_t statusRegister[3];
  /**********************************/
  /* 0 - On/Off  (0 = Off, 1=On)    */
  /* 1 - Device mode (CC, CV ,CP, CR)   */
  /* 2 - Operation/Setup (0/1)      */
  /* */
  /**********************************/
  statusRegister[0] = 0;       // Load Off
  statusRegister[1] = CC_MODE; // Mode
  statusRegister[2] = 0;       // Operation

  // Built-in LED
  pinMode(33, OUTPUT);
  analogWrite(33, 2);
  pinMode(25, OUTPUT);
  analogWrite(25, 25);
  // On/Off outputs
  pinMode(SWITCH_ON_1, OUTPUT);
  pinMode(SWITCH_ON_10, OUTPUT);

  // LCD
  initLCD();

  // Keypad
  keypadBuffer.init();
  myKeypad.setHoldTime(1000);
  myKeypad.addEventListener(keypadEvent); //add an event listener for this keypad

  if (EEPROM.read(8) == 1)
  {
    readFromEEPROM(0);
  }
  else
  {
    //Default values
    setupValues[0] = 1000; // 1.000A CC
    setupValues[1] = 100;  // 1.0V CV
    setupValues[2] = 10;   // 1.0W CP
    setupValues[3] = 100;  // 10.0 Ohm CR
    setupValues[6] = 100;  // 1.0V
  }

  // Showing startup screen
  showStartupScreen();
  delay(2000);

  // Definition of the screen
  mainScreen.addTextField(0, "Cur", 0, 0, 11, "I:", "A ;", CURR_VALUE);
  mainScreen.addTextField(1, "Vol", 0, 1, 11, "U:", "V ;", VOLT_VALUE);
  mainScreen.addTextField(2, "Pow", 0, 2, 11, "P:", "W ;", POWER_VALUE);
  mainScreen.addTextField(3, "Res", 0, 3, 11, "R:", "- ;", RESIST_VALUE);
  mainScreen.addTextField(4, "Tim", 11, 0, 9, "", "", TIME_VALUE);
  mainScreen.addTextField(5, "Ah", 11, 1, 9, "", "Ah", CURR_VALUE);
  mainScreen.addTextField(6, "Vco", 11, 2, 9, "Vc:", "V ", CVOLT_VALUE);
  mainScreen.addTextField(7, "Mode", 11, 3, 3, "", ";", STRING_VALUE);
  mainScreen.addTextField(8, "XXX", 14, 3, 3, "", ";", STRING_VALUE);
  mainScreen.addTextField(9, "Load", 17, 3, 3, "", "", STRING_VALUE);
  mainScreen.setActive();
  mainScreen.drawAll(lcd);

  startTimeMeasure = 0;
  measurePeriod = 290;
  startTimeRefresh = 0;
  refreshPeriod = 260;
  startTimeBlink = 0;
  blinkPeriod = 500;
  startTimeKeyboardBuffer = 0;
  startTimeKeyboard = 0;
  keyboardBufferPeriod = 50;
}

/*******************************************************************************/
/*                                    loop()                                   */
/*******************************************************************************/
void loop()
{

  if ((millis() - startTimeMeasure) > measurePeriod)
  {
    startTimeMeasure = millis();
    measureValues();
  }

  if ((millis() - startTimeBlink) > blinkPeriod)
  {
    startTimeBlink = millis();
    blinkB();
  }

  if ((millis() - startTimeRefresh) > refreshPeriod)
  {
    startTimeRefresh = millis();
    refreshScreen();
  }

  if ((millis() - startTimeKeyboardBuffer) > keyboardBufferPeriod)
  {
    startTimeKeyboard = millis();
    fillKeyboardBuffer();
  }

  if ((millis() - startTimeKeyboard) > keyboardPeriod)
  {
    startTimeKeyboard = millis();
    checkKeyboardBuffer();
  }
}
