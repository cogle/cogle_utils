#pragma once

#include <type_traits>

namespace cogle {

namespace utils {

namespace traits {

template <typename F, typename... Args>
using invoke_result_t = typename std::invoke_result<F, Args...>::type;

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

// todo make generic by using tuple and ... Args
template <typename>
struct FirstArgStruct;

template <typename Ret, typename... Args>
struct FirstArgStruct<Ret (*)(Args...)> {
    using args_tup = std::tuple<Args...>;
};

template <typename Ret, typename F, typename... Args>
struct FirstArgStruct<Ret (F::*)(Args...)> {
    using args_tup = std::tuple<Args...>;
};

template <typename Ret, typename F, typename... Args>
struct FirstArgStruct<Ret (F::*)(Args...) const> {
    using args_tup = std::tuple<Args...>;
};

template <typename F>
using first_argument_t = typename std::tuple_element_t<0, typename FirstArgStruct<decltype(&F::operator())>::args_tup>;

}  // namespace traits

}  // namespace utils
}  // namespace cogle