// Dynamic data buffer and management

#ifndef ZWUTILS_IDF8266_DATABUF_H
#define ZWUTILS_IDF8266_DATABUF_H

#include "stdio.h"
#include <vector>
#include <functional>

#include "esp_err.h"

namespace zw::esp8266::utils {

using DataBuf = std::vector<uint8_t>;

template <typename BufType, class... Args>
const char* PrintToDataBuf(BufType& buf, const char* fmt, Args&&... args) {
  size_t str_len = 12;
  do {
    if (buf.size() < str_len) buf.resize(str_len);
    str_len = snprintf((char*)&buf.front(), buf.size(), fmt, std::forward<Args>(args)...) + 1;
  } while (str_len > buf.size());
  return (char*)&buf.front();
}

// Provide life time management for multiple data buffers.
// Useful for cases where multiple pieces of DataBuf need to be kept alive.
// Such as responding custom HTTP headers.
class DataBufStash {
  using PrepCallback = std::function<esp_err_t(DataBuf&)>;

 public:
  DataBuf& Allocate(size_t init_size = 0) { return cache_.emplace_back(init_size); }

  // Allocate a value entry, and immediately use it in the callback.
  // If callback returns an error, the value is immediately freed.
  // Will forward callback's return value.
  inline esp_err_t AllocAndPrep(size_t init_size, PrepCallback callback) {
    esp_err_t result = callback(Allocate(init_size));
    if (result != ESP_OK) cache_.pop_back();
    return result;
  }

  const std::vector<DataBuf>& cache() const { return cache_; }

 protected:
  std::vector<DataBuf> cache_;
};

}  // namespace zw::esp8266::utils

#endif  // ZWUTILS_IDF8266_DATABUF_H