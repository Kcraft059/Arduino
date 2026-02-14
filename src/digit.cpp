#include <digit.h>

Digit::Digit() {
  pinMode(this->pinout.a, OUTPUT);
  pinMode(this->pinout.b, OUTPUT);
  pinMode(this->pinout.c, OUTPUT);
  pinMode(this->pinout.d, OUTPUT);
  pinMode(this->pinout.e, OUTPUT);
  pinMode(this->pinout.f, OUTPUT);
  pinMode(this->pinout.g, OUTPUT);
  pinMode(this->pinout.DP, OUTPUT);
  pinMode(this->pinout.BR, OUTPUT);

  const char_7seg default_encodings[] = {
      {'0', 0b00111111},
      {'1', 0b00000110},
      {'2', 0b01011011},
      {'3', 0b01001111},
      {'4', 0b01100110},
      {'5', 0b01101101},
      {'6', 0b01111101},
      {'7', 0b00000111},
      {'8', 0b01111111},
      {'9', 0b01101111},
      {'a', 0b01110111}, // A
      {'b', 0b01111100}, // b (looks like lowercase b)
      {'c', 0b00111001}, // C
      {'d', 0b01011110}, // d
      {'e', 0b01111001}, // E
      {'f', 0b01110001}, // F
      {'g', 0b00111101}, // G
      {'h', 0b01110110}, // H
      {'i', 0b00000110}, // same as 1
      {'j', 0b00011110}, // J
      {'k', 0b01110110}, // same as H (no good K on 7seg)
      {'l', 0b00111000}, // L
      {'m', 0b00110111}, // approximated (n-ish)
      {'n', 0b01010100}, // approximated (n)
      {'o', 0b00111111}, // same as 0
      {'p', 0b01110011}, // P
      {'q', 0b01100111}, // q-ish
      {'r', 0b01010000}, // r
      {'s', 0b01101101}, // same as 5
      {'t', 0b01111000}, // t
      {'u', 0b00111110}, // U
      {'v', 0b00111110}, // same as U
      {'w', 0b00101010}, // approximated
      {'x', 0b01110110}, // same as H
      {'y', 0b01101110}, // Y
      {'z', 0b01011011}, // same as 2
      {'.', 0b10000000},
      {'-', 0b01000000}};

  this->encoding.size = 38;
  this->encoding.array = (char_7seg*)malloc(this->encoding.size * sizeof(char_7seg));
  memcpy(this->encoding.array, default_encodings, this->encoding.size * sizeof(char_7seg));
}

Digit::~Digit() {
  free(this->encoding.array);
}

bool Digit::showChar(char chr) {
  for (uint8_t i = 0; i < this->encoding.size; i++)
    if (this->encoding.array[i].chr == chr) {
      this->showRaw(this->encoding.array[i].encoding);
      return true;
    }

  return false;
}

void Digit::showRaw(uint8_t encoding) {
  digitalWrite(this->pinout.a, encoding & 1 << 0);
  digitalWrite(this->pinout.b, encoding & 1 << 1);
  digitalWrite(this->pinout.c, encoding & 1 << 2);
  digitalWrite(this->pinout.d, encoding & 1 << 3);
  digitalWrite(this->pinout.e, encoding & 1 << 4);
  digitalWrite(this->pinout.f, encoding & 1 << 5);
  digitalWrite(this->pinout.g, encoding & 1 << 6);
  digitalWrite(this->pinout.DP, encoding & 1 << 7);
};

void Digit::setBrightness(uint8_t val) {
  analogWrite(this->pinout.BR, val);
}