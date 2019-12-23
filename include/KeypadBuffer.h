
#include <Arduino.h>

class KeypadBuffer {

public:
  KeypadBuffer();
  uint8_t _maxIndex;
  void init();
  void pushChar(char);
  char getChar();
  void clear();

private:
  char buffer[10];
};
