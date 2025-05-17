// RAII based resource life-cycle management

#ifndef ZWUTILS_IDF8266_AUTORELEASE_H
#define ZWUTILS_IDF8266_AUTORELEASE_H

#include <functional>

namespace zw::esp8266::utils {

class AutoRelease {
  using _class = AutoRelease;

 public:
  using ReleaseFunc = std::function<void()>;

  AutoRelease(ReleaseFunc&& rel) : rel_(std::move(rel)) {}
  ~AutoRelease() {
    if (rel_) rel_();
  }

  AutoRelease(const _class&) = delete;
  AutoRelease& operator=(const _class&) = delete;

  AutoRelease(_class&& in) { *this = std::move(in); }
  AutoRelease& operator=(_class&& in) {
    rel_ = std::move(in.rel_);
    return *this;
  }

  void Drop(void) { rel_ = {}; }

 private:
  ReleaseFunc rel_;
};

template <typename T>
class AutoReleaseRes {
  using _class = AutoReleaseRes;

 public:
  using ReleaseFunc = std::function<void(T&&)>;

  AutoReleaseRes() : res_(T()), rel_({}) {}
  AutoReleaseRes(T&& res, ReleaseFunc rel) : res_(std::move(res)), rel_(rel) {}
  ~AutoReleaseRes() { Reset(); }

  void Reset(void) {
    if (rel_) rel_(Drop());
  }

  AutoReleaseRes(const _class&) = delete;
  AutoReleaseRes& operator=(const _class&) = delete;

  AutoReleaseRes(_class&& in) { *this = std::move(in); }
  AutoReleaseRes& operator=(_class&& in) {
    Reset();
    res_ = std::move(in.Swap(T()));
    rel_ = std::move(in.rel_);
    return *this;
  }

  AutoReleaseRes& operator=(T&& res_in) {
    Reset();
    res_ = std::move(res_in);
    return *this;
  }

  const T& operator*() const { return res_; }
  const T* operator->() const { return &res_; }

  T& operator*() { return res_; }
  T* operator->() { return &res_; }

  T Swap(T&& new_res) {
    T ret = std::move(res_);
    res_ = std::move(new_res);
    return ret;
  }

  T Drop(void) { return Swap(T()); }

 private:
  T res_;
  ReleaseFunc rel_;
};

}  // namespace zw::esp8266::utils

#endif  // ZWUTILS_IDF8266_AUTORELEASE_H