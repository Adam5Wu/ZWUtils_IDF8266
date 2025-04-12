// Convenience macros

#ifndef ZWUTILS_IDF8266_MACROS_H
#define ZWUTILS_IDF8266_MACROS_H

#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)
#define UNIQUE_VAR(prefix) prefix##__COUNTER__

//---------------------------
// Error handling
//---------------------------

// Assume #include "esp_err.h"
// Assume #include "esp_log.h" if NDEBUG is not defined
// Assume #include "ZWIDFLTH.hpp"

#ifdef NDEBUG
#define ZW_RETURN_ON_ERROR(expression, cleanup, ret_clean) \
  {                                                        \
    esp_err_t __err_rc = (expression);                     \
    cleanup;                                               \
    if (__err_rc != ESP_OK) {                              \
      ret_clean;                                           \
      return __err_rc;                                     \
    }                                                      \
  }
#else
#define ZW_RETURN_ON_ERROR(expression, cleanup, ret_clean)                                  \
  {                                                                                         \
    esp_err_t __err_rc = (expression);                                                      \
    cleanup;                                                                                \
    if (__err_rc != ESP_OK) {                                                               \
      ESP_LOGW(TAG, "ESP Error (%s:%d) %d (0x%x)", __FILE__, __LINE__, __err_rc, __err_rc); \
      ret_clean;                                                                            \
      return __err_rc;                                                                      \
    }                                                                                       \
  }
#endif

#ifdef NDEBUG
#define ZW_BREAK_ON_ERROR_SIMPLE(expression) \
  {                                          \
    esp_err_t __err_rc = (expression);       \
    if (__err_rc != ESP_OK) {                \
      break;                                 \
    }                                        \
  }
#else
#define ZW_BREAK_ON_ERROR_SIMPLE(expression)                                                \
  {                                                                                         \
    esp_err_t __err_rc = (expression);                                                      \
    if (__err_rc != ESP_OK) {                                                               \
      ESP_LOGW(TAG, "ESP Error (%s:%d) %d (0x%x)", __FILE__, __LINE__, __err_rc, __err_rc); \
      break;                                                                                \
    }                                                                                       \
  }
#endif

#ifdef NDEBUG
#define ZW_BREAK_ON_ERROR(expression, cleanup, ret_clean) \
  {                                                       \
    esp_err_t __err_rc = (expression);                    \
    cleanup;                                              \
    if (__err_rc != ESP_OK) {                             \
      ret_clean;                                          \
      break;                                              \
    }                                                     \
  }
#else
#define ZW_BREAK_ON_ERROR(expression, cleanup, ret_clean)                                   \
  {                                                                                         \
    esp_err_t __err_rc = (expression);                                                      \
    cleanup;                                                                                \
    if (__err_rc != ESP_OK) {                                                               \
      ESP_LOGW(TAG, "ESP Error (%s:%d) %d (0x%x)", __FILE__, __LINE__, __err_rc, __err_rc); \
      ret_clean;                                                                            \
      break;                                                                                \
    }                                                                                       \
  }
#endif

#define ZW_GOTO_ON_ERROR(expression, cleanup, tag) \
  {                                                \
    esp_err_t __status = (expression);             \
    cleanup;                                       \
    ESP_GOTO_ON_ERROR(__status, tag);              \
  }

// ---------------------------
// Event waiting
// ---------------------------

// Assume #include "freertos/FreeRTOS.h"
// Assume #include "freertos/event_groups.h"

#define ZW_WAIT_FOR_EVENTS(event_group, event_bits, reset, all, timeout, on_event, otherwise) \
  {                                                                                           \
    EventBits_t bits = xEventGroupWaitBits(event_group, event_bits, reset, all, timeout);     \
    if (bits & (event_bits)) {                                                                \
      on_event;                                                                               \
    } else {                                                                                  \
      otherwise;                                                                              \
    }                                                                                         \
  }

#define ZW_BLOCK_FOR_EVENTS(event_group, event_bits, on_event) \
  ZW_WAIT_FOR_EVENTS(event_group, event_bits, true, true, portMAX_DELAY, on_event, )

#endif  // ZWUTILS_IDF8266_MACROS_H