#pragma once

#include <utils/abort.hxx>
#include <utils/location.hxx>
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

// TODO MOVE __FUNCTION__ and __LINE__ into their own class
constexpr void assert_ok(const ResultTag tag,
                         const location::SourceLocation& sl = location::SourceLocation::current()) {
    abort::cogle_assert(tag == ResultTag::OK, "Expected OK tag to be valid ", sl);
}

constexpr void assert_err(const ResultTag tag,
                          const location::SourceLocation& sl = location::SourceLocation::current()) {
    abort::cogle_assert(tag == ResultTag::ERR, "Expected ERR tag to be valid ", sl);
}

template <typename R, typename E, typename Enabled = void>
class ResultStorage {
public:
    explicit constexpr ResultStorage(const Ok<R>& ok) noexcept(std::is_nothrow_copy_constructible<R>())
        : tag_(ResultTag::OK), result_(ok.get_result()) {}
    explicit constexpr ResultStorage(const Ok<R>&& ok) noexcept(std::is_nothrow_move_constructible<R>())
        : tag_(ResultTag::OK), result_(std::move(ok.get_result())) {}

    explicit constexpr ResultStorage(const Err<E>& err) noexcept(std::is_nothrow_copy_constructible<E>())
        : tag_(ResultTag::ERR), error_(err.get_error()) {}
    explicit constexpr ResultStorage(const Err<E>&& err) noexcept(std::is_nothrow_move_constructible<E>())
        : tag_(ResultTag::ERR), error_(std::move(err.get_error())) {}

    ~ResultStorage() = default;

    [[nodiscard]] constexpr ResultTag& get_tag() { return tag_; }

    [[nodiscard]] constexpr E& get_error() & noexcept {
        assert_err(tag_);
        return error_;
    }

    [[nodiscard]] constexpr E&& get_error() && noexcept {
        assert_err(tag_);
        return std::move(error_);
    }

    [[nodiscard]] constexpr const E& get_error() const& noexcept {
        assert_err(tag_);
        return error_;
    }

    [[nodiscard]] constexpr const E&& get_error() const&& noexcept {
        assert_err(tag_);
        return std::move(error_);
    }

    [[nodiscard]] constexpr R& get_result() & noexcept {
        assert_ok(tag_);
        return result_;
    }

    [[nodiscard]] constexpr R&& get_result() && noexcept {
        assert_ok(tag_);
        return std::move(result_);
    }

    [[nodiscard]] constexpr const R& get_result() const& noexcept {
        assert_ok(tag_);
        return result_;
    }

    [[nodiscard]] constexpr const R&& get_result() const&& noexcept {
        assert_ok(tag_);
        return std::move(result_);
    }

private:
    ResultTag tag_;

    union {
        R result_;
        E error_;
    };

    template <typename Rv, typename Ev>
    friend class result::Result;
};

// TODO COPY AND PASTE
template <typename R, typename E>
class ResultStorage<R, E,
                    std::enable_if_t<!std::is_trivially_destructible_v<R> || !std::is_trivially_destructible_v<E>>> {
public:
    explicit constexpr ResultStorage(const Ok<R>& ok) noexcept(std::is_nothrow_copy_constructible<R>())
        : tag_(ResultTag::OK), result_(ok.get_result()) {}
    explicit constexpr ResultStorage(const Ok<R>&& ok) noexcept(std::is_nothrow_move_constructible<R>())
        : tag_(ResultTag::OK), result_(std::move(ok.get_result())) {}

    explicit constexpr ResultStorage(const Err<E>& err) noexcept(std::is_nothrow_copy_constructible<E>())
        : tag_(ResultTag::ERR), error_(err.get_error()) {}
    explicit constexpr ResultStorage(const Err<E>&& err) noexcept(std::is_nothrow_move_constructible<E>())
        : tag_(ResultTag::ERR), error_(std::move(err.get_error())) {}

    ~ResultStorage() noexcept {
        switch (tag_) {
            case ResultTag::OK:
                result_.~R();
                break;
            case ResultTag::ERR:
                error_.~E();
                break;
            default:
                break;
        }
    }

    // TODO MOVE and COPY ASSIGNMENT

    [[nodiscard]] constexpr ResultTag& get_tag() { return tag_; }

    [[nodiscard]] constexpr E& get_error() & noexcept {
        assert_err(tag_);
        return error_;
    }

    [[nodiscard]] constexpr E&& get_error() && noexcept {
        assert_err(tag_);
        return std::move(error_);
    }

    [[nodiscard]] constexpr const E& get_error() const& noexcept {
        assert_err(tag_);
        return error_;
    }

    [[nodiscard]] constexpr const E&& get_error() const&& noexcept {
        assert_err(tag_);
        return std::move(error_);
    }

    [[nodiscard]] constexpr R& get_result() & noexcept {
        assert_ok(tag_);
        return result_;
    }

    [[nodiscard]] constexpr R&& get_result() && noexcept {
        assert_ok(tag_);
        return std::move(result_);
    }

    [[nodiscard]] constexpr const R& get_result() const& noexcept {
        assert_ok(tag_);
        return result_;
    }

    [[nodiscard]] constexpr const R&& get_result() const&& noexcept {
        assert_ok(tag_);
        return std::move(result_);
    }

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

    constexpr Ok(Ok&&)    = default;
    constexpr Ok& operator=(Ok&&) = default;

    constexpr Ok(Ok const&) = default;
    constexpr Ok& operator=(Ok const&) = default;

    // https://foonathan.net/2018/03/rvalue-references-api-guidelines/
    [[nodiscard]] constexpr R& get_result() & noexcept { return value_; }
    [[nodiscard]] constexpr R&& get_result() && noexcept { return std::move(value_); }

    [[nodiscard]] constexpr const R& get_result() const& noexcept { return value_; }
    [[nodiscard]] constexpr const R&& get_result() const&& noexcept { return std::move(value_); }

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

    constexpr Err(Err&&)   = default;
    constexpr Err& operator=(Err&&) = default;

    constexpr Err(Err const&) = default;
    constexpr Err& operator=(Err const&) = default;

    [[nodiscard]] constexpr E& get_error() & noexcept { return error_; }
    [[nodiscard]] constexpr E&& get_error() && noexcept { return std::move(error_); }

    [[nodiscard]] constexpr const E& get_error() const& noexcept { return error_; }
    [[nodiscard]] constexpr const E&& get_error() const&& noexcept { return std::move(error_); }

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
    using error_type  = E;

    [[nodiscard]] constexpr Result(const Ok<R>& ok) noexcept(std::is_nothrow_copy_constructible<Storage>())
        : storage_(ok) {}

    [[nodiscard]] constexpr Result(const Ok<R>&& ok) noexcept(std::is_nothrow_move_constructible<Storage>())
        : storage_(std::move(ok)) {}

    [[nodiscard]] constexpr Result(const Err<E>& err) noexcept(std::is_nothrow_copy_constructible<Storage>())
        : storage_(err) {}

    [[nodiscard]] constexpr Result(const Err<E>&& err) noexcept(std::is_nothrow_move_constructible<Storage>())
        : storage_(std::move(err)) {}

    ~Result() = default;

    constexpr Result(Result&&) = default;
    constexpr Result& operator=(Result&&) = default;

    constexpr Result(Result const&) = default;
    constexpr Result& operator=(Result const&) = default;

    [[nodiscard]] constexpr bool is_ok() { return storage_.tag_ == TagEnum::OK; }

    [[nodiscard]] constexpr bool is_err() { return storage_.tag_ == TagEnum::ERR; }

    [[nodiscard]] constexpr E& error() & noexcept { return storage_.get_error(); }

    [[nodiscard]] constexpr E&& error() && noexcept { return std::move(storage_.get_error()); }

    [[nodiscard]] constexpr const E& error() const& noexcept { return storage_.get_error(); }

    [[nodiscard]] constexpr const E&& error() const&& noexcept { return std::move(storage_.get_error()); }

    [[nodiscard]] constexpr R& result() & noexcept { return storage_.get_result(); }

    [[nodiscard]] constexpr R&& result() && noexcept { return std::move(storage_.get_result()); }

    [[nodiscard]] constexpr const R& result() const& noexcept { return storage_.get_result(); }

    [[nodiscard]] constexpr const R&& result() const&& noexcept { return std::move(storage_.get_result()); }

    // Helpful link about auto vs decltype(auto)
    // https://stackoverflow.com/questions/21369113/what-is-the-difference-between-auto-and-decltypeauto-when-returning-from-a-fun
    template <typename F>
    [[nodiscard]] constexpr auto and_then(
        F&& func) & -> Result<typename traits::invoke_result_t<F&&, R&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&, R&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&, R&&>::error_type, E>);

        return and_then_(storage_, std::forward<F>(func));
    }

    template <typename F>
    [[nodiscard]] constexpr auto and_then(
        F&& func) && -> Result<typename traits::invoke_result_t<F&&, R&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&, R&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&, R&&>::error_type, E>);

        return and_then_(std::move(storage_), std::forward<F>(func));
    }

    template <typename F>
    [[nodiscard]] constexpr auto and_then(
        F&& func) const& -> Result<typename traits::invoke_result_t<F&&, R&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&, R&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&, R&&>::error_type, E>);

        return and_then_(storage_, std::forward<F>(func));
    }

    template <typename F>
    [[nodiscard]] constexpr auto and_then(
        F&& func) const&& -> Result<typename traits::invoke_result_t<F&&, R&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&, R&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&, R&&>::error_type, E>);

        return and_then_(std::move(storage_), std::forward<F>(func));
    }

private:
    template <typename S, typename F>
    [[nodiscard]] constexpr auto and_then_(
        S&& s, F&& func) -> Result<typename traits::invoke_result_t<F&&, R&&>::result_type, E> {
        if (is_ok()) {
            return func(std::forward<S>(s).get_result());
        } else {
            return Err<E>{std::forward<S>(s).get_error()};
        }
    }

    Storage storage_;
};

}  // namespace result
}  // namespace utils
}  // namespace cogle