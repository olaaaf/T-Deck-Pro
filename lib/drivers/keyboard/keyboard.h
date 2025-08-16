#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../driver.h"
#include "str.h"
#include <cstdint>
#include <stddef.h>

enum modifier_keys : char {
  SHIFT = 1,
  ALT = 2,
  SYMS = 3,
  BACKSPACE = 4,
  MIC = 5,
  DOLLAR = 6,
  RETURN = 7,
  EMPTY = 0
};

bool is_modifier(char c);
enum STATE : uint8_t { PRESSED = 0x01, RELEASED = 0, NOTHING = 2 };

template <size_t buffer_size> struct keyboard : device {
  keyboard() : state{NOTHING}, buffer_length{0} {}
  void update() final;
  void init() final;
  size_t flush();
  util::str get_and_flush();

private:
  STATE state;
  void inline convert_key(char &c, uint8_t row, uint8_t col);
  char inline get_key();
  bool add_to_buffer(char c);
  bool is_modifier_pressed(const modifier_keys);
  bool change_modifier(char c);
  uint8_t pressed_modifiers;
  char text_buffer[buffer_size];
  size_t buffer_length;
};

#endif
