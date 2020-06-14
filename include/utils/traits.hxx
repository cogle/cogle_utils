#pragma once

#include <type_traits>

namespace cogle {

namespace utils {

namespace traits {

template <typename T, typename U, typename = void>
struct is_comparable_with : std::false_type {};

template <typename T, typename U>
struct is_comparable_with<
    T, U,
    std::void_t<decltype(std::declval<std::remove_const_t<T>> == std::declval<std::remove_const_t<U>> &&
                         std::declval<std::remove_const_t<U>> == std::declval<std::remove_const_t<T>> &&
                         std::declval<std::remove_const_t<T>> != std::declval<std::remove_const_t<U>> &&
                         std::declval<std::remove_const_t<U>> != std::declval<std::remove_const_t<T>>)>>
    : std::true_type {};

}  // namespace traits

}  // namespace utils
}  // namespace cogle