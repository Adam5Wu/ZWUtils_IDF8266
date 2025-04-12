
#include <stddef.h>
#include <string.h>
#include <optional>

#include "esp_err.h"
#include "esp_log.h"

#include "FreeRTOS.h"
#include "freertos/event_groups.h"

#include "ZWUtils.hpp"

namespace zw::esp8266::utils::testing {
namespace {

inline constexpr char TAG[] = "Main";

size_t failed_count = 0;

#define TEST_ASSERT(expr)                                              \
  {                                                                    \
    const char* prefix = __FILE__ ":" ZW_STRINGIFY(__LINE__);          \
    do {                                                               \
      if (expr) break;                                                 \
      ESP_LOGI(TAG, "[%s] %-25s assertion failed!", prefix, __func__); \
      return ESP_FAIL;                                                 \
    } while (0);                                                       \
  }

#define TEST_RUN(expr)                                                                  \
  {                                                                                     \
    const char* prefix = __FILE__ ":" ZW_STRINGIFY(__LINE__);                           \
    int case_num = __COUNTER__;                                                         \
    const char* verdict = "PASSED";                                                     \
    do {                                                                                \
      if (expr) break;                                                                  \
      ++failed_count;                                                                   \
      verdict = "FAILED";                                                               \
    } while (0);                                                                        \
    ESP_LOGI(TAG, "[%s] %-25s case #%02d ... %s", prefix, __func__, case_num, verdict); \
  }

esp_err_t _test_ZWStrings() {
  TEST_RUN(STRLEN(TAG) == 4);
  TEST_RUN(PasswordRedact(TAG) == "M**n");

  return ESP_OK;
}

esp_err_t _test_ZWIDFLTH() {
  TEST_RUN([] {
    ESP_RETURN_ON_ERROR(ESP_OK);
    return ESP_FAIL;
  }() == ESP_FAIL);
  TEST_RUN([] {
    ESP_RETURN_ON_ERROR(ESP_FAIL);
    return ESP_OK;
  }() == ESP_FAIL);

  TEST_RUN([] {
    ESP_GOTO_ON_ERROR(ESP_OK, failed);
    return ESP_OK;
  failed:
    return ESP_FAIL;
  }() == ESP_OK);
  TEST_RUN([] {
    ESP_GOTO_ON_ERROR(ESP_FAIL, failed);
    return ESP_OK;
  failed:
    return ESP_FAIL;
  }() == ESP_FAIL);

  return ESP_OK;
}

esp_err_t _test_ZWMacros_Basic() {
  {
    bool unconditional_cleanup_run = false;
    bool failure_cleanup_run = false;
    TEST_RUN([&] {
      ZW_RETURN_ON_ERROR(ESP_OK, unconditional_cleanup_run = true, failure_cleanup_run = true);
      return ESP_OK;
    }() == ESP_OK);
    TEST_RUN(unconditional_cleanup_run);
    TEST_RUN(!failure_cleanup_run);
  }
  {
    bool unconditional_cleanup_run = false;
    bool failure_cleanup_run = false;
    TEST_RUN([&] {
      ZW_RETURN_ON_ERROR(ESP_FAIL, unconditional_cleanup_run = true, failure_cleanup_run = true);
      return ESP_OK;
    }() == ESP_FAIL);
    TEST_RUN(unconditional_cleanup_run);
    TEST_RUN(failure_cleanup_run);
  }

  TEST_RUN([&] {
    do {
      ZW_BREAK_ON_ERROR_SIMPLE(ESP_OK);
      return ESP_OK;
    } while (0);
    return ESP_FAIL;
  }() == ESP_OK);
  TEST_RUN([&] {
    do {
      ZW_BREAK_ON_ERROR_SIMPLE(ESP_FAIL);
      return ESP_OK;
    } while (0);
    return ESP_FAIL;
  }() == ESP_FAIL);

  {
    bool unconditional_cleanup_run = false;
    bool failure_cleanup_run = false;
    TEST_RUN([&] {
      do {
        ZW_BREAK_ON_ERROR(ESP_OK, unconditional_cleanup_run = true, failure_cleanup_run = true);
        return ESP_OK;
      } while (0);
      return ESP_FAIL;
    }() == ESP_OK);
    TEST_RUN(unconditional_cleanup_run);
    TEST_RUN(!failure_cleanup_run);
  }
  {
    bool unconditional_cleanup_run = false;
    bool failure_cleanup_run = false;
    TEST_RUN([&] {
      do {
        ZW_BREAK_ON_ERROR(ESP_FAIL, unconditional_cleanup_run = true, failure_cleanup_run = true);
        return ESP_OK;
      } while (0);
      return ESP_FAIL;
    }() == ESP_FAIL);
    TEST_RUN(unconditional_cleanup_run);
    TEST_RUN(failure_cleanup_run);
  }

  {
    bool unconditional_cleanup_run = false;
    TEST_RUN([&] {
      ZW_GOTO_ON_ERROR(ESP_OK, unconditional_cleanup_run = true, failed);
      return ESP_OK;
    failed:
      return ESP_FAIL;
    }() == ESP_OK);
    TEST_RUN(unconditional_cleanup_run);
  }
  {
    bool unconditional_cleanup_run = false;
    TEST_RUN([&] {
      ZW_GOTO_ON_ERROR(ESP_FAIL, unconditional_cleanup_run = true, failed);
      return ESP_OK;
    failed:
      return ESP_FAIL;
    }() == ESP_FAIL);
    TEST_RUN(unconditional_cleanup_run);
  }

  return ESP_OK;
}

esp_err_t _test_ZWAutoRelease() {
  {
    bool released = false;
    {
      AutoRelease TestRel([&] { released = true; });
      TEST_ASSERT(released == false);
    }
    TEST_RUN(released == true);
  }

  {
    bool released = false;
    int release_secret = 0;
    {
      AutoReleaseRes<int> TestRel(123, [&](int&& x) {
        released = true;
        release_secret = x;
      });
      TEST_ASSERT(released == false);
      TEST_ASSERT(release_secret == 0);
    }
    TEST_RUN(released == true);
    TEST_RUN(release_secret == 123);
  }

  return ESP_OK;
}

esp_err_t _test_ZWDataOrError() {
  {
    DataOrError<std::string> Test("Test");
    TEST_RUN(Test.error() == ESP_OK);
    TEST_RUN(Test == true);
    TEST_RUN(*Test == "Test");
    TEST_RUN(Test->length() == 4);
  }
  {
    DataOrError<std::string> Test(ESP_FAIL);
    TEST_RUN(Test.error() == ESP_FAIL);
    TEST_RUN(Test == false);
  }
  {
    DataOrError<std::string> Test = std::string("Test");
    TEST_RUN(Test.error() == ESP_OK);
    TEST_RUN(Test == true);
    TEST_RUN(*Test == "Test");
    TEST_RUN(Test->length() == 4);
    Test = ESP_FAIL;
    TEST_RUN(Test.error() == ESP_FAIL);
    TEST_RUN(Test == false);
    Test = std::string("OK");
    TEST_RUN(Test.error() == ESP_OK);
    TEST_RUN(Test == true);
    TEST_RUN(*Test == "OK");
    TEST_RUN(Test->length() == 2);
  }

  return ESP_OK;
}

esp_err_t _test_ZWMacros_EventWait() {
  AutoReleaseRes<EventGroupHandle_t> TestEvents(xEventGroupCreate(), [](EventGroupHandle_t&& x) {
    if (x != NULL) vEventGroupDelete(x);
  });
  TEST_ASSERT(*TestEvents != NULL);

  {
    std::optional<bool> event_signaled = std::nullopt;
    ZW_WAIT_FOR_EVENTS(*TestEvents, BIT0, false, false, 1, event_signaled = true,
                       event_signaled = false);
    TEST_ASSERT(event_signaled.has_value());
    TEST_RUN(*event_signaled == false);
  }

  xEventGroupSetBits(*TestEvents, BIT0);
  {
    std::optional<bool> event_signaled = std::nullopt;
    ZW_WAIT_FOR_EVENTS(*TestEvents, BIT0, false, false, 1, event_signaled = true,
                       event_signaled = false);
    TEST_ASSERT(event_signaled.has_value());
    TEST_RUN(*event_signaled == true);
    TEST_RUN((xEventGroupGetBits(*TestEvents) & BIT0) != 0);
  }

  {
    std::optional<bool> event_signaled = std::nullopt;
    ZW_WAIT_FOR_EVENTS(*TestEvents, BIT0, true, false, 1, event_signaled = true,
                       event_signaled = false);
    TEST_ASSERT(event_signaled.has_value());
    TEST_RUN(*event_signaled == true);
    TEST_RUN((xEventGroupGetBits(*TestEvents) & BIT0) == 0);
  }

  return ESP_OK;
}

esp_err_t _test_DataBuf() {
  {
    DataBuf buf;
    const char* text = buf.PrintTo("Test %d", 123);
    TEST_ASSERT(text != NULL);
    TEST_RUN(strcmp(text, "Test 123") == 0);
  }
  {
    DataBuf buf(20);
    TEST_ASSERT(buf.size() == 20);
    const char* text = buf.PrintTo("Test %d", 123);
    TEST_ASSERT(text != NULL);
    TEST_RUN(strcmp(text, "Test 123") == 0);
    TEST_RUN(buf.size() == 20);
  }
  {
    DataBufStash stash;
    DataBuf& buf1 = stash.Allocate();
    TEST_ASSERT(stash.cache().size() == 1);
    const char* text = buf1.PrintTo("Test %d", 123);
    TEST_ASSERT(text != NULL);
    TEST_RUN(strcmp(text, "Test 123") == 0);

    const char* text2;
    stash.AllocAndPrep(20, [&](DataBuf& buf) {
      text2 = buf.PrintTo("Test %d", 234);
      return text2 != NULL ? ESP_OK : ESP_FAIL;
    });
    TEST_RUN(strcmp(text2, "Test 234") == 0);
    TEST_RUN(stash.cache().size() == 2);

    stash.AllocAndPrep(20, [&](DataBuf& buf) {
      // The allocated buffer will be released
      return ESP_FAIL;
    });
    TEST_RUN(stash.cache().size() == 2);

    DataBuf& buf2 = stash.Allocate(20);
    TEST_ASSERT(buf2.size() == 20);
    TEST_RUN(stash.cache().size() == 3);
  }

  return ESP_OK;
}

esp_err_t _run() {
  if (_test_ZWStrings() != ESP_OK) return ESP_FAIL;
  if (_test_ZWIDFLTH() != ESP_OK) return ESP_FAIL;
  if (_test_ZWMacros_Basic() != ESP_OK) return ESP_FAIL;
  if (_test_ZWAutoRelease() != ESP_OK) return ESP_FAIL;
  if (_test_ZWDataOrError() != ESP_OK) return ESP_FAIL;
  if (_test_ZWMacros_EventWait() != ESP_OK) return ESP_FAIL;
  if (_test_DataBuf() != ESP_OK) return ESP_FAIL;

  return ESP_OK;
}

}  // namespace
}  // namespace zw::esp8266::utils::testing

using namespace zw::esp8266::utils::testing;

extern "C" void app_main(void) {
  ESP_LOGI(TAG, "ZWUtils for ESP8266 IDF testing");

  if (_run() != ESP_OK) goto failed;
  ESP_LOGE(TAG, "All tests complete, %d failed.", failed_count);
  return;

failed:
  ESP_LOGE(TAG, "Test failed to complete!");
}