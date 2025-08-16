#include "error.h"
#include <Arduino.h>
#include <cstdarg>

namespace helpers {
const char INFO = 'I';
const char DEBUG = 'D';
const char ERROR = 'E';
const char LEVELS[3]{ERROR, DEBUG, INFO};
} // namespace helpers

namespace util {

const void logger::initialize() {}

const void logger::error(const error_t &msg) {
#if (LOG_LEVEL >= 1)
  write(logger_name, LOG::ERROR, msg);
#endif
}
const void logger::debug(const error_t &msg) {
#if (LOG_LEVEL >= 2)
  write(logger_name, LOG::DEBUG, msg);
#endif
}
const void logger::info(const error_t &msg) {
#if (LOG_LEVEL >= 3)
  write(logger_name, LOG::INFO, msg);
#endif
}

const void logger::write(const char *logger_name, const LOG msg_level,
                         const error_t &message) {
  Serial.printf("%c - %s: %s\n", helpers::LEVELS[msg_level], logger_name,
                message.data);
}
} // namespace util
