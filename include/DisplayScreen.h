
#include <LiquidCrystal_I2C.h>
#include "TextField.h"

class DisplayScreen {
public:
  DisplayScreen();
  void addTextField(uint8_t, String, uint8_t, uint8_t, uint8_t, String, String);
  void drawAll(LiquidCrystal_I2C& lcd, uint8_t countField = 10);
  void drawTextField(uint8_t, LiquidCrystal_I2C& lcd);
  TextField textFields[10] = {
    TextField("",0,0,0,"",""),
    TextField("",0,0,0,"",""),
    TextField("",0,0,0,"",""),
    TextField("",0,0,0,"",""),
    TextField("",0,0,0,"",""),
    TextField("",0,0,0,"",""),
    TextField("",0,0,0,"",""),
    TextField("",0,0,0,"",""),
    TextField("",0,0,0,"",""),
    TextField("",0,0,0,"","")
  };
};
