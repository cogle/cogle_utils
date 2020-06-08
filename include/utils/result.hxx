#pragma once

#include <type_traits>

namespace cogle {

namespace utils {
namespace result {

namespace detail {
enum class ResultTag { OK = 0, ERR = 1 };
}  // namespace detail

template <typename R>
struct Ok {
  using value_type = R;

  [[nodiscard]] constexpr Ok(R val) noexcept(std::is_nothrow_copy_constructible<R>()) : value_(val) {}
  [[nodiscard]] constexpr Ok(R&& val) noexcept(std::is_nothrow_move_constructible<R>()) : value_(std::move(val)) {}

 private:
  R value_;
};

template <typename T, typename E>
class Result;

template <typename T, typename E>
class Result {};

}  // namespace result
}  // namespace utils
}  // namespace cogle