
#include <LiquidCrystal_I2C.h>
#include "TextField.h"

class DisplayScreen {
public:
  DisplayScreen();
  bool _isActive;
  bool _refreshEnabled;
  void addTextField(uint8_t, String, uint8_t, uint8_t, uint8_t, String, String, uint8_t);
  void drawAll(LiquidCrystal_I2C& lcd);
  void drawTextField(uint8_t, LiquidCrystal_I2C& lcd);
  void refresh(LiquidCrystal_I2C& lcd, int8_t pos = -1);
  void setActive();
  void setInactive();
  TextField textFields[10] = {
    TextField("",false,0,0,0,"","",STRING_VALUE),
    TextField("",false,0,0,0,"","",STRING_VALUE),
    TextField("",false,0,0,0,"","",STRING_VALUE),
    TextField("",false,0,0,0,"","",STRING_VALUE),
    TextField("",false,0,0,0,"","",STRING_VALUE),
    TextField("",false,0,0,0,"","",STRING_VALUE),
    TextField("",false,0,0,0,"","",STRING_VALUE),
    TextField("",false,0,0,0,"","",STRING_VALUE),
    TextField("",false,0,0,0,"","",STRING_VALUE),
    TextField("",false,0,0,0,"","",STRING_VALUE)
  };

};
