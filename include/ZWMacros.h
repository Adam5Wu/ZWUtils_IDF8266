// Convenience macros

#ifndef ZWUTILS_IDF8266_MACROS_H
#define ZWUTILS_IDF8266_MACROS_H

#define ZW_STRINGIFY(x) __STRING(x)
#define ZW_UNIQUE_VAR(prefix) __CONCAT(prefix, __COUNTER__)

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
      ESP_LOGD(TAG, "ESP Error (%s:%d) %d (0x%x)", __FILE__, __LINE__, __err_rc, __err_rc); \
      ret_clean;                                                                            \
      return __err_rc;                                                                      \
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
      ESP_LOGD(TAG, "ESP Error (%s:%d) %d (0x%x)", __FILE__, __LINE__, __err_rc, __err_rc); \
      ret_clean;                                                                            \
      break;                                                                                \
    }                                                                                       \
  }
#endif

#define ZW_BREAK_ON_ERROR_SIMPLE(expression) ZW_BREAK_ON_ERROR(expression, , )

#define ZW_GOTO_ON_ERROR(expression, cleanup, tag) \
  {                                                \
    esp_err_t __status = (expression);             \
    cleanup;                                       \
    ESP_GOTO_ON_ERROR(__status, tag);              \
  }

//---------------------------
// Event waiting
//---------------------------

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

//---------------------------
// Synchronization
//---------------------------

// Assume #include "freertos/FreeRTOS.h"
// Assume #include "freertos/semphr.h"
// Assume #include "ZWAutoRelease.hpp"

#define ZW_ACQUIRE_FOR_SCOPE(lock_obj, delay, otherwise) \
  if (xSemaphoreTake(lock_obj, delay) != pdTRUE) {       \
    otherwise;                                           \
  }                                                      \
  ::zw::esp8266::utils::AutoRelease ZW_UNIQUE_VAR(lock_releaser)([&] { xSemaphoreGive(lock_obj); })

#define ZW_ACQUIRE_FOR_SCOPE_SIMPLE(lock_obj) ZW_ACQUIRE_FOR_SCOPE(lock_obj, portMAX_DELAY, )

#define ZW_RECURSIVE_ACQUIRE_FOR_SCOPE(lock_obj, delay, otherwise) \
  if (xSemaphoreTakeRecursive(lock_obj, delay) != pdTRUE) {        \
    otherwise;                                                     \
  }                                                                \
  ::zw::esp8266::utils::AutoRelease ZW_UNIQUE_VAR(lock_releaser)(  \
      [&] { xSemaphoreGiveRecursive(lock_obj); })

#define ZW_RECURSIVE_ACQUIRE_FOR_SCOPE_SIMPLE(lock_obj) \
  ZW_RECURSIVE_ACQUIRE_FOR_SCOPE(lock_obj, portMAX_DELAY, )

#endif  // ZWUTILS_IDF8266_MACROS_H