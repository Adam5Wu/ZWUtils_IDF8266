// A lousy clone of Abseil StatusOr :P

#ifndef ZWUTILS_IDF8266_DATAORERROR_H
#define ZWUTILS_IDF8266_DATAORERROR_H

#include <variant>
#include <utility>

#include "ZWMacros.h"

namespace zw::esp8266::utils {

class ESPErrorStatus {
 public:
  const esp_err_t value;
  const std::string message;

  ESPErrorStatus() : ESPErrorStatus(ESP_OK, "") {}
  ESPErrorStatus(esp_err_t value) : ESPErrorStatus(value, "") {}
  ESPErrorStatus(const std::string& message) : ESPErrorStatus(ESP_FAIL, message) {}
  ESPErrorStatus(esp_err_t value, const std::string& message) : value(value), message(message) {}

  operator bool() const { return value == ESP_OK; }
};

template <typename T>
class DataOrError {
 public:
  DataOrError() : DataOrError(ESP_FAIL) {}
  DataOrError(T&& data) { *this = std::move(data); }
  DataOrError(esp_err_t error) { *this = error; }

  DataOrError& operator=(T&& data) {
    data_or_error_ = std::variant<T, esp_err_t>{std::in_place_index<0>, std::move(data)};
    return *this;
  }
  DataOrError& operator=(esp_err_t error) {
    assert(error != ESP_OK && "ESP_OK is not an error, supply data instead!");
    data_or_error_ = std::variant<T, esp_err_t>{std::in_place_index<1>, error};
    return *this;
  }

  DataOrError(const DataOrError&) = delete;
  DataOrError& operator=(const DataOrError&) = delete;

  DataOrError(DataOrError&& in) { *this = std::move(in); }
  DataOrError& operator=(DataOrError&& in) {
    return data_or_error_ = std::move(in.data_or_error_), *this;
  }

  T& operator*() { return std::get<0>(data_or_error_); }
  const T& operator*() const { return std::get<0>(data_or_error_); }
  T* operator->() { return &std::get<0>(data_or_error_); }
  const T* operator->() const { return &std::get<0>(data_or_error_); }

  operator bool() const { return data_or_error_.index() == 0; }
  esp_err_t error() const {
    return data_or_error_.index() == 1 ? std::get<1>(data_or_error_) : ESP_OK;
  }

 private:
  std::variant<T, esp_err_t> data_or_error_;
};

#define ASSIGN_OR_RETURN(val, statement)                                          \
  auto ZW_UNIQUE_VAR(data_or_error) = (statement);                                \
  if (!ZW_UNIQUE_VAR(data_or_error)) return ZW_UNIQUE_VAR(data_or_error).error(); \
  val = std::move(*ZW_UNIQUE_VAR(data_or_error))

}  // namespace zw::esp8266::utils

#endif  // ZWUTILS_IDF8266_DATAORERROR_H