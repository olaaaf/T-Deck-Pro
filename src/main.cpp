#include <Arduino.h>
#include <util.h>

#include <keyboard/keyboard.h>

keyboard<static_cast<size_t>(128)> k;
void setup() {
  Serial.begin(BAUDRATE);
  k.init();
}

void loop() {
	k.update();
}
