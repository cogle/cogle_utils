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
class ResultStorage {
public:
    explicit constexpr ResultStorage(const Ok<R>& ok) noexcept(std::is_nothrow_copy_constructible<R>())
        : tag_(ResultTag::OK), result_(ok.get_result()) {}
    explicit constexpr ResultStorage(const Ok<R>&& ok) noexcept(std::is_nothrow_move_constructible<R>())
        : tag_(ResultTag::OK), result_(std::move(ok.get_result())) {}

    explicit constexpr ResultStorage(const Err<E>& err) noexcept(std::is_nothrow_copy_constructible<E>())
        : tag_(ResultTag::ERR), result_(err.get_error()) {}
    explicit constexpr ResultStorage(const Err<E>&& err) noexcept(std::is_nothrow_move_constructible<E>())
        : tag_(ResultTag::ERR), result_(std::move(err.get_error())) {}

    [[nodiscard]] constexpr ResultTag& get_tag() { return tag_; }

private:
    ResultTag tag_;

    union {
        R result_;
        E error_;
    };

    template <typename Rv, typename Ev>
    friend class result::Result;
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
    using TagEnum = detail::ResultTag;

    using Storage = detail::ResultStorage<R, E>;

public:
    using result_type = R;
    using error_type = E;

    [[nodiscard]] constexpr Result(const Ok<R>& ok) noexcept(std::is_nothrow_copy_constructible<Storage>())
        : storage_(ok) {}

    [[nodiscard]] constexpr Result(const Ok<R>&& ok) noexcept(std::is_nothrow_move_constructible<Storage>())
        : storage_(std::move(ok)) {}

    [[nodiscard]] constexpr Result(const Err<E>& err) noexcept(std::is_nothrow_copy_constructible<Storage>())
        : storage_(err) {}

    [[nodiscard]] constexpr Result(const Err<E>&& err) noexcept(std::is_nothrow_move_constructible<Storage>())
        : storage_(std::move(err)) {}

    [[nodiscard]] constexpr bool is_ok() { return storage_.tag_ == TagEnum::OK; }

    [[nodiscard]] constexpr bool is_err() { return storage_.tag_ == TagEnum::ERR; }

    // Helpful link about auto vs decltype(auto)
    // https://stackoverflow.com/questions/21369113/what-is-the-difference-between-auto-and-decltypeauto-when-returning-from-a-fun

    // TODO allow just Ok as return type as well in other set of overloads
    /*
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
    */

private:
    Storage storage_;
};

}  // namespace result
}  // namespace utils
}  // namespace cogle