#pragma once

#include <utils/traits.hxx>

namespace cogle {

namespace utils {
namespace result {

// Forward declare Ok
template <typename R>
struct Ok;

// Forward declare Err
template <typename E>
struct Err;

// Forward declare Result
template <typename R, typename E>
class Result;

namespace detail {
enum class ResultTag { OK = 0, ERR = 1 };

template <typename Result>
using extract_ok_t = typename Result::result_type;

template <typename Result>
using extract_err_t = typename Result::error_type;

template <typename R, typename E, typename = void>
class ResultStorage  {
public:
    ResultStorage();

private:
    ResultTag tag_;

    union {
        R value_;
        E error_;
    };

template <typename Rv, typename Ev>
friend class Result;
};

}  // namespace detail

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

    // https://foonathan.net/2018/03/rvalue-references-api-guidelines/
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

    template <typename Rv, typename Ev>
    friend class Result;
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

    template <typename Rv, typename Ev>
    friend class Result;
};

template <typename R, typename E>
class Result {
    using result_tag = detail::ResultTag;

public:
    using result_type = R;
    using error_type = E;

    [[nodiscard]] constexpr Result(const Ok<R>& ok) noexcept(std::is_nothrow_copy_constructible<Ok<R>>())
        : tag_(result_tag::OK), ok_value_(ok.value_) {}

    [[nodiscard]] constexpr Result(const Ok<R>&& ok) noexcept(std::is_nothrow_move_constructible<Ok<R>>())
        : tag_(result_tag::OK), ok_value_(std::move(ok.value_)) {}

    [[nodiscard]] constexpr Result(const Err<E>& err) noexcept(std::is_nothrow_copy_constructible<Err<E>>())
        : tag_(result_tag::ERR), err_value_(err.error_) {}

    [[nodiscard]] constexpr Result(const Err<E>&& err) noexcept(std::is_nothrow_move_constructible<Err<E>>())
        : tag_(result_tag::ERR), err_value_(std::move(err.error_)) {}

    ~Result() {
        if (is_ok()) {
            ok_value_.~R();
        } else {
            err_value_.~E();
        }
    }

    [[nodiscard]] constexpr bool is_ok() { return tag_ == result_tag::OK; }

    [[nodiscard]] constexpr bool is_err() { return tag_ == result_tag::ERR; }

    // Helpful link about auto vs decltype(auto)
    // https://stackoverflow.com/questions/21369113/what-is-the-difference-between-auto-and-decltypeauto-when-returning-from-a-fun

    // TODO allow just Ok as return type as well in other set of overloads
    template <typename F>
    [[nodiscard]] constexpr auto and_then(F&& func)
        -> Result<detail::extract_ok_t<traits::invoke_result_t<F&&, R&&>>, E> {
        static_assert(traits::is_invocable_v<F&&, R&&>);

        if (is_ok()) {
            // Ensure if is error then is same
            return func(ok_value_);
        } else {
            return Err<E>{err_value_};
        }
    }

private:
    detail::ResultTag tag_;

    union {
        R ok_value_;
        E err_value_;
    };
};

}  // namespace result
}  // namespace utils
}  // namespace cogle