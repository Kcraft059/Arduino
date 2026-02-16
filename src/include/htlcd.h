#ifndef HTLCD
#define HTLCD

#include <Arduino.h>
#include <Wire.h>

#define I2C_DELAY_US 500
#define MAX_SCROLL_TEXTS 4

class WireDevice {
protected:
  const uint8_t adress;

public:
  WireDevice(uint8_t adress);

  uint8_t sendCmd(uint8_t cmd, uint8_t* data, uint8_t datal);
};

struct chrPosition {
  uint8_t line;
  uint8_t column;
};

struct scrollText {
  char* str = NULL;
  unsigned long _timemark = 0;
  struct chrPosition pos = {1, 1};
  uint16_t dly = 500;
  enum {
    LEFT,
    RIGHT
  } dir = RIGHT;
  uint8_t len = 0;
  uint8_t _strsize = 0;
};

struct lcdType {
  uint8_t lines;
  uint8_t columns;
};

template <typename TS, typename TH>
struct handle {
  TS s;
  TH h;
};

class HTlcd : public WireDevice {
private:
  struct handle<struct scrollText, uint8_t*> scrollTexts[MAX_SCROLL_TEXTS];
  const struct {
    uint8_t printStr = 1;
    uint8_t moveCursor = 2;
    uint8_t clearAll = 4;
    uint8_t setType = 5;
    uint8_t brightness = 7;
    uint8_t printChr = 10;
  } commands;

public:
  struct lcdType type;

public:
  HTlcd(struct lcdType type);
  HTlcd(uint8_t adress, struct lcdType type);

  
  // Abstract
  void refresh();
  uint8_t* addScrollText(struct scrollText text);
  uint8_t delScrollText(uint8_t handle);
  
  // Direct commands
  uint8_t clearAll();
  uint8_t printStr(char* str);
  uint8_t printChr(char chr);
  uint8_t moveCursor(struct chrPosition pos);
  uint8_t setBrightness(uint8_t brightness);
  
private:
  uint8_t setType(struct lcdType* type);
  uint8_t updateScrollText(struct scrollText* text);
};

#endif