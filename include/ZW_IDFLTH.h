// A Low-to-high adapter macro layer providing symbol compatibility
// with high version ESP-IDFs.

#ifndef ZWUTILS_IDF8266_IDFLTH_H
#define ZWUTILS_IDF8266_IDFLTH_H

// Assume #include "esp_err.h"
// Assume #include "esp_log.h" if NDEBUG is not defined

#ifdef NDEBUG
#define ESP_RETURN_ON_ERROR(x) \
  do {                         \
    esp_err_t __err_rc = (x);  \
    if (__err_rc != ESP_OK) {  \
      return __err_rc;         \
    }                          \
  } while (0)
#else
#define ESP_RETURN_ON_ERROR(x)                                                              \
  do {                                                                                      \
    esp_err_t __err_rc = (x);                                                               \
    if (__err_rc != ESP_OK) {                                                               \
      ESP_LOGW(TAG, "ESP Error (%s:%d) %d (0x%x)", __FILE__, __LINE__, __err_rc, __err_rc); \
      return __err_rc;                                                                      \
    }                                                                                       \
  } while (0)
#endif  // NDEBUG

#ifdef NDEBUG
#define ESP_GOTO_ON_ERROR(x, goto_tag) \
  do {                                 \
    esp_err_t __err_rc = (x);          \
    if (__err_rc != ESP_OK) {          \
      goto goto_tag;                   \
    }                                  \
  } while (0)
#else
#define ESP_GOTO_ON_ERROR(x, goto_tag)                                                      \
  do {                                                                                      \
    esp_err_t __err_rc = (x);                                                               \
    if (__err_rc != ESP_OK) {                                                               \
      ESP_LOGW(TAG, "ESP Error (%s:%d) %d (0x%x)", __FILE__, __LINE__, __err_rc, __err_rc); \
      goto goto_tag;                                                                        \
    }                                                                                       \
  } while (0)
#endif  // NDEBUG

#endif  // ZWUTILS_IDF8266_IDFLTH_H