// Misc string handling convenience utilities

#ifndef ZWUTILS_IDF8266_STRINGS_H
#define ZWUTILS_IDF8266_STRINGS_H

#include <string>

namespace zw::esp8266::utils {

// Compile-time length for constexpr strings
template <size_t N>
constexpr size_t STRLEN(char const (&)[N]) {
  return N - 1;
}

// Redact password by keeping only the first and last character.
inline std::string PasswordRedact(const std::string& input) {
  std::string result(input.length(), '*');
  if (!input.empty()) {
    result.front() = input.front();
    result.back() = input.back();
  }
  return result;
}

}  // namespace zw::esp8266::utils

#endif  // ZWUTILS_IDF8266_STRINGS_H