#ifndef ERROR_H
#define ERROR_H

#include "str.h"
namespace util {

enum LOG : uint8_t {
  ERROR = 1,
  DEBUG,
  INFO,
};

#ifndef LOG_LEVEL
#define LOG_LEVEL 3 
#endif

struct error_t {
  error_t() {};
  error_t(const char *m) : data{m} , len{1}{}
  error_t(const char *msg, size_t len) : data{msg}, len{len} {}
  bool is_error() const { return len; };
  const char* data;
  size_t len;
};

struct logger {
  logger(const char *name) : logger_name{name} {}
  const void error(const error_t &);
  const void debug(const error_t &);
  const void info(const error_t &);

private:
  static const void write(const char *, const LOG, const error_t &message);
  static const void initialize();
  const char *logger_name;
};

} // namespace frt
#endif // ERROR_H
