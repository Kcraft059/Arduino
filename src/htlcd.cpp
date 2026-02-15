#include <htlcd.h>

WireDevice::WireDevice(uint8_t adress) : adress(adress) {};

uint8_t WireDevice::sendCmd(uint8_t cmd, uint8_t* data, uint8_t datal) {
  Wire.beginTransmission(this->adress);
  Wire.write(cmd);

  for (uint8_t i = 0; i < datal; i++)
    Wire.write(data[i]);

  uint8_t result = Wire.endTransmission();
  delayMicroseconds(I2C_DELAY_US);
  return result;
}

HTlcd::HTlcd(struct lcdType type) : WireDevice(58), type(type) {
  this->setType(&this->type);
  this->scrollTexts[0].h = NULL;
};

HTlcd::HTlcd(uint8_t adress, struct lcdType type) : WireDevice(adress), type(type) {
  this->setType(&this->type);
  this->scrollTexts[0].h = NULL;
};

uint8_t HTlcd::setType(struct lcdType* type) {
  uint8_t data[2] = {
      type->lines,
      type->columns};
  return WireDevice::sendCmd(this->commands.setType, data, 2);
};

uint8_t HTlcd::setBrightness(uint8_t brightness) {
  if (brightness > 250) return ~0;

  uint8_t data[1] = {brightness};
  return WireDevice::sendCmd(this->commands.brightness, data, 1);
}

uint8_t HTlcd::clearAll() {
  return WireDevice::sendCmd(this->commands.clearAll, NULL, 0);
}

uint8_t HTlcd::printStr(char* str) {
  return WireDevice::sendCmd(this->commands.printStr, (uint8_t*)str, strlen(str));
}

uint8_t HTlcd::printChr(char chr) {
  uint8_t data = (uint8_t)chr;
  return WireDevice::sendCmd(this->commands.printChr, &data, 1);
}

uint8_t HTlcd::moveCursor(struct chrPosition pos) {
  uint8_t data[2] = {pos.line, pos.column};
  return WireDevice::sendCmd(this->commands.moveCursor, data, 2);
};

void HTlcd::refresh() {
  for (uint8_t i = 0; this->scrollTexts[i].h; i++)
    if (millis() >= this->scrollTexts[i].s._timemark)
      this->updateScrollText(&(this->scrollTexts[i].s));
}

uint8_t* HTlcd::addScrollText(struct scrollText text) {

  // Free index search
  uint8_t free_index = 0;
  while (this->scrollTexts[free_index].h && free_index < MAX_SCROLL_TEXTS) // Search for a free index in the scroll text list
    free_index++;

  if (free_index >= MAX_SCROLL_TEXTS) return NULL;     // If no free index return NULL
  uint8_t* handle = (uint8_t*)malloc(sizeof(uint8_t)); //
  *handle = free_index + 1;

  // String handling
  if (!text.str) return NULL;
  text._strsize = strlen(text.str);
  char* txtBuffer = (char*)malloc(text._strsize + 1); // Let space for char shift

  switch (text.dir) {
  case scrollText::RIGHT:
    memcpy(txtBuffer, text.str, text._strsize);
    break;

  case scrollText::LEFT:
    for (uint8_t i = 0; i < text._strsize; i++)
      txtBuffer[text._strsize - 1 - i] = text.str[i];
    break;

  default:
    return NULL;
  }

  text.str = txtBuffer;

  // Config
  text._timemark = millis() ? millis() - 1 : 0; // Prevent overflow on negative
  if (!text.len) text.len = this->type.columns; // Set the default visible length to the display size

  this->scrollTexts[free_index].s = text;
  this->scrollTexts[free_index].h = handle;

  if (free_index + 1 < MAX_SCROLL_TEXTS)
    this->scrollTexts[free_index + 1].h = NULL;

  return handle;
}

uint8_t HTlcd::delScrollText(uint8_t handle) {
  handle -= 1;
  if (handle >= MAX_SCROLL_TEXTS) return 1;

  free(this->scrollTexts[handle].s.str);
  free(this->scrollTexts[handle].h);

  char str[this->scrollTexts[handle].s.len + 1];
  for (uint8_t i = 0; i < this->scrollTexts[handle].s.len; i++)
    str[i] = ' ';
  str[this->scrollTexts[handle].s.len] = '\0';

  this->moveCursor(this->scrollTexts[handle].s.pos);
  this->printStr(str);

  uint8_t offset = 0;
  for (uint8_t i = handle + 1; i < MAX_SCROLL_TEXTS && this->scrollTexts[i].h; i++) {
    *(this->scrollTexts[i].h) -= 1;
    offset++;
  }

  if (offset) memmove(this->scrollTexts + handle,
                      this->scrollTexts + handle + 1,
                      offset * sizeof(::handle<struct scrollText, uint8_t*>));
  this->scrollTexts[handle + offset].h = NULL;

  return 0;
}

uint8_t HTlcd::updateScrollText(struct scrollText* text) {
  text->_timemark = millis() + text->dly;

  memmove(text->str + 1, text->str, text->_strsize * sizeof(char)); // Move to right by one char
  memcpy(text->str, text->str + text->_strsize, 1 * sizeof(char));  // Move last char back to beginning of str

  this->moveCursor(text->pos); // Move cursor to starting pos;

  for (uint8_t i = 0; i < text->_strsize && i < this->type.columns && i < text->len; i++)       // Iterate over string, in the limit given by the display
    this->printChr(text->str[(text->dir == scrollText::RIGHT) ? i : (text->_strsize - 1 - i)]); // If left reverse string

  return 0;
}