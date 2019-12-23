
#include "KeypadBuffer.h"

KeypadBuffer::KeypadBuffer()
{
  _maxIndex = 0;
}

void KeypadBuffer::init() {
  _maxIndex = 0;
  /*
  for (uint8_t i = 0; i < 10; i++) {
    buffer[i] = 0;
  }
  */
}

void KeypadBuffer::pushChar(char c) {
  if (_maxIndex <= 9) {                  // _maxIndex ukazuje na nejblizsi volnou pozici v bufferu
    buffer[_maxIndex] = c;
    _maxIndex++;
  }
}

char KeypadBuffer::getChar() {
  if (_maxIndex > 0) {                  // pokud _maxIndex ukazuje na 0, buffer je prazdny
    char out = buffer[0];
    for (uint8_t i = 1; i <= _maxIndex; i++) {
      buffer[i-1] = buffer[i];
    }
    _maxIndex--;
    return out;
  } else {
    return 255;
  }
}
