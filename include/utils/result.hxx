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

    [[nodiscard]] explicit constexpr Ok(const R& val) noexcept(std::is_nothrow_copy_constructible<R>()) : value_(val) {}
    [[nodiscard]] explicit constexpr Ok(R&& val) noexcept(std::is_nothrow_move_constructible<R>())
        : value_(std::move(val)) {}

    [[nodiscard]] constexpr R& get_result() & noexcept { return value_; }
    [[nodiscard]] constexpr R&& get_result() && noexcept { return std::move(value_); }

    [[nodiscard]] constexpr const R& get_result() const& noexcept { return value_; }
    [[nodiscard]] constexpr const R&& get_result() const&& noexcept { return value_; }

    template <typename T>
    [[nodiscard]] constexpr bool operator==(Ok<T> const& o) const {
        return value_ == o.value_;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator!=(Ok<T> const& o) const {
        return value_ != o.value_;
    }

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