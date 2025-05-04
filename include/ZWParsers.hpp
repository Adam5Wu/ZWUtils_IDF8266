
// Misc parsing convenience utilities

#ifndef ZWUTILS_IDF8266_PARSERS_H
#define ZWUTILS_IDF8266_PARSERS_H

#include <string>

#include "esp_err.h"

#include "ZWDataOrError.hpp"

namespace zw::esp8266::utils {

inline int8_t ParseHex(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;  // Error
}

inline DataOrError<std::string> UrlDecode(const std::string& in_str) {
  std::string out(in_str.size(), '\0');
  const char* in_ptr = in_str.data();

  size_t out_len = 0;
  while (*in_ptr != '\0') {
    char c = *in_ptr++;
    if (c != '%') {
      out[out_len++] = c;
      continue;
    }

    int8_t h1 = ParseHex(*in_ptr++);
    if (h1 < 0) return ESP_ERR_INVALID_ARG;
    int8_t h2 = ParseHex(*in_ptr++);
    if (h2 < 0) return ESP_ERR_INVALID_ARG;

    out[out_len++] = (h1 << 4) | h2;
  }

  out.resize(out_len);
  return out;
}

}  // namespace zw::esp8266::utils

#endif  // ZWUTILS_IDF8266_PARSERS_H