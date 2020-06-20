#pragma once

#include <utils/traits.hxx>

namespace cogle {

namespace utils {
namespace result {

namespace detail {
enum class ResultTag { OK = 0, ERR = 1 };
}  // namespace detail

// Forward declare Ok
template <typename R>
struct Ok;

template <typename E>
struct Err;

template <typename T, typename E>
class Result;

template <typename R>
struct Ok {
    using value_type = R;

    [[nodiscard]] explicit constexpr Ok(const R& val) noexcept(std::is_nothrow_copy_constructible<R>()) : value_(val) {}
    [[nodiscard]] explicit constexpr Ok(R&& val) noexcept(std::is_nothrow_move_constructible<R>())
        : value_(std::move(val)) {}

    constexpr Ok(Ok&&) = default;
    constexpr Ok& operator=(Ok&&) = default;

    constexpr Ok(Ok const&) = default;
    constexpr Ok& operator=(Ok const&) = default;

    [[nodiscard]] constexpr R& get_result() & noexcept { return value_; }
    [[nodiscard]] constexpr R&& get_result() && noexcept { return std::move(value_); }

    [[nodiscard]] constexpr const R& get_result() const& noexcept { return value_; }
    [[nodiscard]] constexpr const R&& get_result() const&& noexcept { return value_; }

    template <typename T>
    [[nodiscard]] constexpr bool operator==(const Ok<T>& o) const {
        static_assert(traits::is_comparable_with<R, T>{}, "Equality operator requires comparability");
        return value_ == o.value_;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator!=(const Ok<T>& o) const {
        static_assert(traits::is_comparable_with<R, T>{}, "Inequality operator requires comparability");
        return value_ != o.value_;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator==(const Err<T>&) const {
        static_assert(traits::is_comparable_with<R, T>{}, "Equality operator requires comparability");
        return false;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator!=(const Err<T>&) const {
        static_assert(traits::is_comparable_with<R, T>{}, "Inequality operator requires comparability");
        return true;
    }

private:
    R value_;
};

template <typename E>
struct Err {
    using value_type = E;

    [[nodiscard]] explicit constexpr Err(const E& val) noexcept(std::is_nothrow_copy_constructible<E>())
        : error_(val) {}
    [[nodiscard]] explicit constexpr Err(E&& val) noexcept(std::is_nothrow_move_constructible<E>())
        : error_(std::move(val)) {}

    constexpr Err(Err&&) = default;
    constexpr Err& operator=(Err&&) = default;

    constexpr Err(Err const&) = default;
    constexpr Err& operator=(Err const&) = default;

    [[nodiscard]] constexpr E& get_error() & noexcept { return error_; }
    [[nodiscard]] constexpr E&& get_error() && noexcept { return std::move(error_); }

    [[nodiscard]] constexpr const E& get_error() const& noexcept { return error_; }
    [[nodiscard]] constexpr const E&& get_error() const&& noexcept { return error_; }

    template <typename T>
    [[nodiscard]] constexpr bool operator==(const Err<T>& o) const {
        static_assert(traits::is_comparable_with<E, T>{}, "Equality operator requires comparability");
        return error_ == o.error_;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator!=(const Err<T>& o) const {
        static_assert(traits::is_comparable_with<E, T>{}, "Inequality operator requires comparability");
        return error_ != o.error_;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator==(const Ok<T>&) const {
        static_assert(traits::is_comparable_with<E, T>{}, "Equality operator requires comparability");
        return false;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator!=(const Ok<T>) const {
        static_assert(traits::is_comparable_with<E, T>{}, "Inequality operator requires comparability");
        return true;
    }

private:
    E error_;
};

template <typename R, typename E>
class Result {
    using result_tag = detail::ResultTag;

public:
    using result_type = R;
    using error_type = E;

    [[nodiscard]] explicit constexpr Result(const Ok<R>& ok) noexcept(std::is_nothrow_copy_constructible<Ok<R>>())
        : tag_(result_tag::OK), ok_(ok) {}

    [[nodiscard]] explicit constexpr Result(const Ok<R>&& ok) noexcept(std::is_nothrow_move_constructible<Ok<R>>())
        : tag_(result_tag::OK), ok_(std::move(ok)) {}

    [[nodiscard]] explicit constexpr Result(const Err<E>& err) noexcept(std::is_nothrow_copy_constructible<Err<E>>())
        : tag_(result_tag::ERR), err_(err) {}

    [[nodiscard]] explicit constexpr Result(const Err<E>&& err) noexcept(std::is_nothrow_move_constructible<Err<E>>())
        : tag_(result_tag::ERR), err_(std::move(err)) {}

    constexpr bool is_ok() { return tag_ == result_tag::OK; }

    constexpr bool is_err() { return tag_ == result_tag::ERR; }

private:
    detail::ResultTag tag_;

    union {
        Ok<R> ok_;
        Err<E> err_;
    };
};

}  // namespace result
}  // namespace utils
}  // namespace cogle