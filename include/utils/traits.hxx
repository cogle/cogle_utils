#pragma once

#include <type_traits>

namespace cogle {

namespace utils {

namespace traits {

template <typename F, typename... Args>
using invoke_result_t = typename std::invoke_result<F &&, Args &&...>::type;

template <typename F, typename... Args>
constexpr bool is_invocable_v = std::is_invocable<F &&, Args &&...>::value;

template <typename T, typename U, typename = void>
struct is_comparable_with : std::false_type {};

template <typename T, typename U>
struct is_comparable_with<
    T, U,
    std::void_t<decltype(
        std::declval<std::remove_const_t<T> const &>() == std::declval<std::remove_const_t<U> const &>() &&
        std::declval<std::remove_const_t<U> const &>() == std::declval<std::remove_const_t<T> const &>() &&
        std::declval<std::remove_const_t<T> const &>() != std::declval<std::remove_const_t<U> const &>() &&
        std::declval<std::remove_const_t<U> const &>() != std::declval<std::remove_const_t<T> const &>())>>
    : std::true_type {};

}  // namespace traits

}  // namespace utils
}  // namespace cogle