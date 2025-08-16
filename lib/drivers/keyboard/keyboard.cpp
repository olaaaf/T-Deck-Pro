#include "esp32-hal-gpio.h"
#include "pins_arduino.h"

#include "keyboard.h"
#include <Adafruit_TCA8418.h>
#include <Arduino.h>

#include <cstring>
#include <error.h>

namespace {

const uint8_t BOARD_I2C_ADDR_KEYBOARD = 0x34;
const int KEYPAD_SDA = 13;
const int KEYPAD_SCL = 14;
const uint8_t KEYPAD_IRQ = 15;

const uint8_t KEYPAD_RELEASE_VAL_MIN = 1;
const uint8_t KEYPAD_PRESS_VAL_MIN = 129;
const uint8_t KEYPAD_ROWS = 4;
const uint8_t KEYPAD_COLS = 10;

Adafruit_TCA8418 keypad;
util::logger l{"Keyboard"};
bool new_input = false;
void interruptHandler() { new_input = true; }
uint8_t recentKey;

const char keymap[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', BACKSPACE},
    {ALT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', DOLLAR, RETURN},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, SHIFT, MIC, ' ', SYMS, SHIFT},
};

const char alt_keymap[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'#', '1', '2', '3', '(', ')', '_', '-', '+', '@'},
    {'*', '4', '5', '6', '/', ':', ';', '\'', '"', BACKSPACE},
    {ALT, '7', '8', '9', '?', '!', ',', '.', DOLLAR, '\n'},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, SHIFT, '0', ' ', SYMS, SHIFT},
};

struct key_press {
  char c;
  STATE state;
  key_press(char _c, STATE _state) : c{_c}, state{_state} {}
};

} // namespace

bool is_modifier(char c) {
  return c >= modifier_keys::SHIFT && c <= modifier_keys::RETURN;
}

template <size_t bs> void keyboard<bs>::init() {
  l.info("keyboard initialized");
  Wire.begin(KEYPAD_SDA, KEYPAD_SCL);
  Wire.beginTransmission(BOARD_I2C_ADDR_KEYBOARD);
  Wire.endTransmission(true);
  if (!keypad.begin(BOARD_I2C_ADDR_KEYBOARD, &Wire)) {
    l.error("keypad not found");
    return;
  }

  const bool &validMatrix = keypad.matrix(KEYPAD_ROWS, KEYPAD_COLS);
  if (!validMatrix) {
    l.error("keypad has invalid number of rows/cols");
  }

  pinMode(KEYPAD_IRQ, INPUT);
  attachInterrupt(digitalPinToInterrupt(KEYPAD_IRQ), interruptHandler, CHANGE);
  keypad.flush();
  keypad.enableInterrupts();
}

template <size_t bs> void keyboard<bs>::update() {
  if (new_input) {
    int intstat = keypad.readRegister(TCA8418_REG_INT_STAT);

    if (intstat & 0x02) {
      //  reading the registers is mandatory to clear IRQ flag
      //  can also be used to find the GPIO changed
      //  as these registers are a bitmap of the gpio pins.
      keypad.readRegister(TCA8418_REG_GPIO_INT_STAT_1);
      keypad.readRegister(TCA8418_REG_GPIO_INT_STAT_2);
      keypad.readRegister(TCA8418_REG_GPIO_INT_STAT_3);
      //  clear GPIO IRQ flag
      keypad.writeRegister(TCA8418_REG_INT_STAT, 2);
    }
    keypad.writeRegister(TCA8418_REG_INT_STAT, 1);

    if (keypad.available() > 0) {
      const auto key = get_key();
      if (!add_to_buffer(key)) {
        l.error("keyboard buffer full");
      }
    }
    if ((intstat & 0x01) == 0) {
      new_input = false;
    }
  }
}

template <size_t bs> util::str keyboard<bs>::get_and_flush() {
  util::str buffer{text_buffer};
  flush();
  return buffer;
}

template <size_t bs> inline bool keyboard<bs>::add_to_buffer(char c) {
  if (buffer_length > bs) {
    return false;
  }
  if (c == EMPTY)
    return true;
  if (c == BACKSPACE && buffer_length > 0) {
    buffer_length--;
    text_buffer[buffer_length] = '\0';
  } else {
    text_buffer[buffer_length] = c;
    buffer_length++;
  }
  return true;
}

template <size_t bs> size_t keyboard<bs>::flush() {
  l.debug("flushing the keyboard buffer");
  memset(text_buffer, 0, sizeof(text_buffer));
  buffer_length = 0;
  return 0;
}

template <size_t bs> inline char keyboard<bs>::get_key() {
  static char c, row, col;
  recentKey = keypad.getEvent();
  if (recentKey >= KEYPAD_PRESS_VAL_MIN) { // press event
    recentKey = recentKey - KEYPAD_PRESS_VAL_MIN;
    state = PRESSED;
  } else {
    recentKey = recentKey - KEYPAD_RELEASE_VAL_MIN;
    state = RELEASED;
  }
  if (recentKey == 97 || recentKey == 96)
    return EMPTY;
  row = recentKey / KEYPAD_COLS;
  col = (KEYPAD_COLS - 1) - recentKey % KEYPAD_COLS;
  c = keymap[row][col];
  convert_key(c, row, col);
  if (is_modifier(c)) {
    change_modifier(c);
  }
  if (state == PRESSED) {
    return c;
  }
  return EMPTY;
}

template <size_t bs>
inline void keyboard<bs>::convert_key(char &c, uint8_t row, uint8_t col) {
  if (is_modifier_pressed(SHIFT)) {
    // do uppercase
    c -= 'a' - 'A';
  } else if (is_modifier_pressed(modifier_keys::ALT)) {
    c = alt_keymap[row][col];
  }
}

template <size_t bs>
bool keyboard<bs>::is_modifier_pressed(const modifier_keys modifier) {
  return pressed_modifiers & (0x01 << modifier);
}

template <size_t bs> bool keyboard<bs>::change_modifier(char c) {
  if (state == NOTHING)
    return false;
  pressed_modifiers = (pressed_modifiers & (0xFE << c)) | (state << c);
  return true;
}

template struct keyboard<128>;
