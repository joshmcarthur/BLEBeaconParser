#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

// Minimal Arduino String mock
class String {
 private:
  std::string str;

 public:
  String() : str("") {}
  String(const char* s) : str(s ? s : "") {}
  String(const String& other) : str(other.str) {}

  String& operator=(const char* s) {
    str = s ? s : "";
    return *this;
  }

  String& operator=(const String& other) {
    if (this != &other) {
      str = other.str;
    }
    return *this;
  }

  String& operator+=(const char* s) {
    str += s ? s : "";
    return *this;
  }

  String& operator+=(char c) {
    str += c;
    return *this;
  }

  String& operator+=(const String& other) {
    str += other.str;
    return *this;
  }

  const char* c_str() const {
    return str.c_str();
  }
  size_t length() const {
    return str.length();
  }
  bool operator==(const char* s) const {
    return str == (s ? s : "");
  }
  bool operator!=(const char* s) const {
    return str != (s ? s : "");
  }

  friend String operator+(const String& lhs, const char* rhs) {
    String result = lhs;
    result += rhs;
    return result;
  }

  friend String operator+(const String& lhs, char rhs) {
    String result = lhs;
    result += rhs;
    return result;
  }
};

// Arduino compatibility defines
#define ARDUINO 100
typedef uint8_t byte;

#endif  // MOCK_ARDUINO_H
