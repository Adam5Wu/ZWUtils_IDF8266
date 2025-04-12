# ZWUtils_IDF8266
Utility collection for ESP8266 IDF-style C++ development

Besides a bunch of trivial (but handy) functions and macro snippet,
some notable features of this library are:

## RAII based resource life-cycle management
Provides scope-based life-cycle management for *arbitrary* resources.

- `AutoRelease`: Best for when the resource is abstract, e.g. a device
  needing "turn off", or a collection of variables needing to be set to
  specific values.

  ```
  allocate_some_resource();
  AutoRelease releaser([]{
    // Automatically called when `releaser` goes out-of-scope
    release_some_resource();
  });
  ```

  ```
  x = allocate_resource();
  AutoRelease releaser([&x]{
    // Automatically called when `releaser` goes out-of-scope
    release_resource(x);
  });
  ```

- `AutoReleaseRes<T>`: Best for managing a concrete object. Note that it
  feels similar to `std::unique_ptr`, but is more convenient to use than
  the latter when the object is not managed using the standard `new` /
  `delete` operator.

  ```
  AutoReleaseRes<SomeType> obj(AllocateSomeType(...),
    [](SomeType&& x){
      // Automatically called when `obj` goes out-of-scope
      ReleaseSomeType(x);
    });

  obj->member;
  ...
  ```

## Exception-less error handling
Exceptions are overly expensive for resource-constrained environments such
as ESP8266. The [Abseil](https://github.com/abseil/abseil-cpp) `StatusOr`
provides a very workable solution.

Without porting the entire Abseil library to ESP-IDF, this library provides
a "lousy clone" that captures some of the basic features.

```
DataOrError<SomeType> func_foo(...) {
  SomeType result;
  ...
  ESP_RETURN_ON_ERROR(some_func(...));
  ...
  return result;
}

esp_err_t func_bar(...) {
  ...
  ASSIGN_OR_RETURN(SomeType x, func_foo(...));
  ...
  return ESP_OK;
}
```

## Dynamic data buffer and life-cycle management
Data buffer, especially string buffer is often needed, and this library
provide a thin layer wrapping around `std::vector<uint8_t>`.

### Print formatted string to a dynamic data buffer
```
DataBuf buf;
// Handles expanding buffer size as needed;
// Returns the head of the formatted string buffer
const char* text = buf.PrintTo("some %s format %d string", str, num);
...
// The same buffer can be reused (previous returned str pointer is invalidated)
const char* text2 = buf.PrintTo("some %x other format", num);

DataBuf buf(123);  // Supports pre-allocation to avoid resizing
const char* text3 = buf.PrintTo("some large string: %s", large_str);
```

### Managing a number of data buffers
Sometimes multiple data buffers are needed, such as assigning HTTP response
headers -- the caller must keep all header value buffers valid until the
response is sent.

`DataBufStash` provides an easy way to manage the life-cycle of the buffers.
```
DataBufStash buffers;
auto& buf = buffers.Allocate(init_size);
const char* text = buf.PrintTo("some %s format %d string", str, num);
...

// Or more conveniently with a callback function:
buffers.AllocAndPrep(init_size, [](DataBuf& buf) {
  return prep_data(buf);
})

// Error cascading is supported:
ESP_RETURN_ON_ERROR(buffers.AllocAndPrep(init_size,
  [](DataBuf& buf) {
    ESP_RETURN_ON_ERROR(...);
    ...
    return ESP_OK;
  }));
```