
#define STRING_VALUE 0
#define CURR_VALUE 1
#define VOLT_VALUE 2
#define POWER_VALUE 3
#define RESIST_VALUE 4
#define TIME_VALUE 5

#include <LiquidCrystal_I2C.h>

class TextField {

public:
  String _name;
  bool _isUsed;
  uint8_t _x;
  uint8_t _y;
  uint8_t _length;
  String _prefix;
  String _suffix;
  uint16_t _nvalue;
  String _svalue;
  bool _inEditMode;
  bool _valuePrintable;
  uint8_t _valueType;
  int8_t _numValueLength;
  int8_t _blinkValuePosition;
  int8_t _blink;
  //bool _isValueNew;
  TextField(String, bool, uint8_t, uint8_t, uint8_t, String, String, uint8_t);
  //void setBlinkCursor(uint8_t position);
  //void disableBlinkCursor(uint8_t position);
  void setEditMode(LiquidCrystal_I2C& lcd);
  void disableEditMode(LiquidCrystal_I2C& lcd);
  void draw(LiquidCrystal_I2C& lcd);
  void drawValue(LiquidCrystal_I2C& lcd, int8_t pos = -1);
  void clearValue(LiquidCrystal_I2C& lcd);
  void setValue(uint16_t value);
  void setValue(String value);
  int8_t shiftLeftValueBlinkPosition();
  int8_t shiftRightValueBlinkPosition();

private:
  void printFrac(uint16_t value, uint16_t divider, uint8_t valueLength, uint8_t fracLength, LiquidCrystal_I2C& lcd);
  void setPrefix(String prefix);
  void setSuffix(String suffix);
  void drawPrefix(LiquidCrystal_I2C& lcd);
  void drawSuffix(LiquidCrystal_I2C& lcd);
  uint8_t getPrefixLength();
  uint8_t getSuffixLength();
  void drawNumberValue(LiquidCrystal_I2C& lcd);
  void drawStringValue(LiquidCrystal_I2C& lcd);
};
