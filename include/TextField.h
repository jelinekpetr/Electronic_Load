
#define CURR_VALUE 1
#define VOLT_VALUE 2
#define POWER_VALUE 3
#define RESIST_VALUE 4
#define TIME_VALUE 5


#include <LiquidCrystal_I2C.h>

class TextField {

public:
  uint8_t _x;
  uint8_t _y;
  uint8_t _length;
  String _prefix;
  String _suffix;
  String _name;
  uint16_t _value;
  bool _isEditable;
  TextField(String, uint8_t, uint8_t, uint8_t, String, String);
  void setBlinkCursor(uint8_t position);
  void disableBlinkCursor(uint8_t position);
  void setEditable(LiquidCrystal_I2C& lcd);
  void disableEditable(LiquidCrystal_I2C& lcd);
  void drawPrefix(LiquidCrystal_I2C& lcd);
  void drawSuffix(LiquidCrystal_I2C& lcd, int8_t special = -1);
  void draw(LiquidCrystal_I2C& lcd, int8_t special = -1);
  void updateValue(uint16_t value, uint8_t type, LiquidCrystal_I2C& lcd);
  void updateValue(String value, LiquidCrystal_I2C& lcd);
  uint8_t getDelimiterPosition();

private:
  void printFrac(uint16_t value, uint16_t divider, uint8_t valueLength, uint8_t fracLength, LiquidCrystal_I2C& lcd);
  void setPrefix(String prefix);
  void setSuffix(String suffix);
  uint8_t getPrefixLength();
  uint8_t getSuffixLength();
};
