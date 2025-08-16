#ifndef STR_H
#define STR_H

#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstring>

namespace util {

// THIS CODE IS AI-GENERATED
// sorry

/**
 * @brief Simple heap-owning string type for ESP32-S3.
 *
 * - Owns a dynamically allocated null-terminated buffer.
 * - Provides copy/move semantics.
 * - Supports concatenation (operator<<), indexing, and formatting.
 */
struct str {
  char *data; ///< Pointer to null-terminated character buffer
  size_t len; ///< Number of characters (excluding null terminator)

  /**
   * @brief Default constructor. Creates an empty string.
   */
  str() : data{nullptr}, len{0} {}

  /**
   * @brief Construct from raw buffer and explicit length.
   * @param s Pointer to input buffer.
   * @param n Number of characters to copy.
   */
  str(const char *s, size_t n) : data{nullptr}, len{0} {
    if (s && n > 0) {
      data = new char[n + 1];
      memcpy(data, s, n);
      data[n] = '\0';
      len = n;
    }
  }

  /**
   * @brief Construct from a C-string.
   * @param s Null-terminated string.
   */
  explicit str(const char *s) : data{nullptr}, len{0} {
    if (s) {
      len = std::strlen(s);
      data = new char[len + 1];
      memcpy(data, s, len + 1);
    }
  }

  /**
   * @brief Copy constructor.
   * @param other String to copy from.
   */
  str(const str &other) : data{nullptr}, len{0} {
    if (other.data && other.len > 0) {
      data = new char[other.len + 1];
      memcpy(data, other.data, other.len + 1);
      len = other.len;
    }
  }

  /**
   * @brief Move constructor.
   * @param other String to move from.
   */
  str(str &&other) noexcept : data{other.data}, len{other.len} {
    other.data = nullptr;
    other.len = 0;
  }

  /**
   * @brief Copy assignment operator.
   * @param other String to copy from.
   * @return Reference to this string.
   */
  str &operator=(const str &other) {
    if (this != &other) {
      clear();
      if (other.data && other.len > 0) {
        data = new char[other.len + 1];
        memcpy(data, other.data, other.len + 1);
        len = other.len;
      }
    }
    return *this;
  }

  /**
   * @brief Move assignment operator.
   * @param other String to move from.
   * @return Reference to this string.
   */
  str &operator=(str &&other) noexcept {
    if (this != &other) {
      clear();
      data = other.data;
      len = other.len;
      other.data = nullptr;
      other.len = 0;
    }
    return *this;
  }

  /**
   * @brief Destructor. Frees allocated buffer.
   */
  ~str() { clear(); }

  /**
   * @brief Clear the string and release memory.
   */
  void clear() {
    delete[] data;
    data = nullptr;
    len = 0;
  }

  /**
   * @brief Indexing operator.
   * @param i Index (must be < size()).
   * @return Reference to character at index.
   */
  char &operator[](size_t i) { return data[i]; }

  /**
   * @brief Const indexing operator.
   * @param i Index (must be < size()).
   * @return Const reference to character at index.
   */
  const char &operator[](size_t i) const { return data[i]; }

  /**
   * @brief Append a C-string.
   * @param rhs Null-terminated string.
   * @return Reference to this string.
   */
  str &operator<<(const char *rhs) {
    if (!rhs)
      return *this;
    size_t rhs_len = std::strlen(rhs);
    if (rhs_len == 0)
      return *this;

    char *new_buf = new char[len + rhs_len + 1];
    if (data)
      memcpy(new_buf, data, len);
    memcpy(new_buf + len, rhs, rhs_len + 1);

    delete[] data;
    data = new_buf;
    len += rhs_len;
    return *this;
  }

  /**
   * @brief Append another str.
   * @param rhs String to append.
   * @return Reference to this string.
   */
  str &operator<<(const str &rhs) { return (*this << rhs.c_str()); }

  /**
   * @brief Append a single character.
   * @param c Character to append.
   * @return Reference to this string.
   */
  str &operator<<(char c) {
    char tmp[2] = {c, '\0'};
    return (*this << tmp);
  }

  /**
   * @brief Create a formatted string (printf-style).
   * @param fmt Format string.
   * @param ... Arguments.
   * @return New formatted str.
   */
  static str format(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    // measure required size
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    str result;
    if (needed > 0) {
      result.len = needed;
      result.data = new char[needed + 1];
      vsnprintf(result.data, needed + 1, fmt, args);
    }
    va_end(args);
    return result;
  }

  /**
   * @brief Get pointer to C-string buffer.
   * @return Null-terminated string.
   */
  const char *c_str() const { return data ? data : ""; }

  /**
   * @brief Get string length (number of characters).
   * @return Length.
   */
  size_t size() const { return len; }

  /**
   * @brief Check if string is empty.
   * @return True if empty.
   */
  bool empty() const { return len == 0; }
};

} // namespace util

#endif // STR_H
