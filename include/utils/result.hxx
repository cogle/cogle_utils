#pragma once

#include <new>
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
enum class ResultTag { OK = 0, ERR = 1, INVALID = 2 };

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

    constexpr ResultStorage(ResultStorage const&) = default;

    // TODO noexcept
    constexpr ResultStorage(ResultStorage&& o) : tag_(o.tag_) {
        switch (o.tag_) {
            case ResultTag::OK:
                result_ = std::move(o.result_);
                break;
            case ResultTag::ERR:
                error_ = std::move(o.error_);
                break;
            default:
                break;
        }

        o.invalidate();
    }

    constexpr ResultStorage& operator=(ResultStorage const&) = default;

    // TODO noexcept
    constexpr ResultStorage& operator=(ResultStorage&& o) {
        switch (o.tag_) {
            case ResultTag::OK:
                result_ = std::move(o.result_);
                break;
            case ResultTag::ERR:
                error_ = std::move(o.error_);
                break;
            default:
                break;
        }

        tag_ = o.tag_;
        o.invalidate();
    }

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
    void invalidate() { tag_ = ResultTag::INVALID; }

    ResultTag tag_;

    union {
        R result_;
        E error_;
    };

    template <typename Rv, typename Ev>
    friend class result::Result;
};

template <typename R, typename E>
class ResultStorage<R, E,
                    std::enable_if_t<!std::is_void_v<R> &&
                                     (!std::is_trivially_destructible_v<R> || !std::is_trivially_destructible_v<E>)>> {
public:
    explicit constexpr ResultStorage(const Ok<R>& ok) noexcept(std::is_nothrow_copy_constructible<R>())
        : tag_(ResultTag::OK), result_(ok.get_result()) {}
    explicit constexpr ResultStorage(const Ok<R>&& ok) noexcept(std::is_nothrow_move_constructible<R>())
        : tag_(ResultTag::OK), result_(std::move(ok.get_result())) {}

    explicit constexpr ResultStorage(const Err<E>& err) noexcept(std::is_nothrow_copy_constructible<E>())
        : tag_(ResultTag::ERR), error_(err.get_error()) {}
    explicit constexpr ResultStorage(const Err<E>&& err) noexcept(std::is_nothrow_move_constructible<E>())
        : tag_(ResultTag::ERR), error_(std::move(err.get_error())) {}

    ~ResultStorage() noexcept(noexcept(std::declval<R>().~R()) && noexcept(std::declval<E>().~E())) {
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

    constexpr ResultStorage(ResultStorage const&) = default;

    // TODO noexcept
    constexpr ResultStorage(ResultStorage&& o) : tag_(o.tag_) {
        switch (o.tag_) {
            case ResultTag::OK:
                result_ = std::move(o.result_);
                break;
            case ResultTag::ERR:
                error_ = std::move(o.error_);
                break;
            default:
                break;
        }

        o.invalidate();
    }

    constexpr ResultStorage& operator=(ResultStorage const&) = default;

    constexpr ResultStorage& operator=(ResultStorage&& o) {
        switch (o.tag_) {
            case ResultTag::OK:
                result_ = std::move(o.result_);
                break;
            case ResultTag::ERR:
                error_ = std::move(o.error_);
                break;
            default:
                break;
        }

        tag_ = o.tag_;
        o.invalidate();
    }

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
    void invalidate() noexcept {
        clear();
        tag_ = ResultTag::INVALID;
    }

    void clear() noexcept {
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

    ResultTag tag_;

    union {
        R result_;
        E error_;
    };

    template <typename Rv, typename Ev>
    friend class result::Result;
};

template <typename E>
class ResultStorage<void, E, std::enable_if_t<std::is_standard_layout_v<E> && std::is_trivial_v<E>>> {
    using type = typename std::aligned_storage<sizeof(E), alignof(E)>::type;

public:
    explicit constexpr ResultStorage(const Ok<void>&) noexcept : tag_(ResultTag::OK) {}

    explicit constexpr ResultStorage(const Ok<void>&&) noexcept : tag_(ResultTag::OK) {}

    explicit constexpr ResultStorage(const Err<E>& err) noexcept(std::is_nothrow_copy_constructible<E>())
        : tag_(ResultTag::ERR) {
        new (&error_) E(err.get_error());
    }
    explicit constexpr ResultStorage(const Err<E>&& err) noexcept(std::is_nothrow_move_constructible<E>())
        : tag_(ResultTag::ERR) {
        new (&error_) E(std::move(err.get_error()));
    }

    ~ResultStorage() {
        switch (tag_) {
            case ResultTag::OK:
                break;
            case ResultTag::ERR:
                std::launder(reinterpret_cast<E*>(&error_))->~E();
                break;
            default:
                break;
        }
    }

    constexpr ResultStorage& operator=(const ResultStorage& o) noexcept(std::is_nothrow_copy_constructible<E>()) {
        switch (o.tag_) {
            case ResultTag::OK:
                break;
            case ResultTag::ERR:
                new (&error_) E(*std::launder(reinterpret_cast<E*>(&o.error_)));
                break;
            default:
                break;
        }

        tag_ = o.tag_;

        return *this;
    }

    constexpr ResultStorage& operator=(ResultStorage&& o) noexcept(std::is_nothrow_move_constructible<E>()) {
        switch (o.tag_) {
            case ResultTag::OK:
                break;
            case ResultTag::ERR:
                new (&error_) E(std::move(*std::launder(reinterpret_cast<E*>(&o.error_))));
                o.tag_ = ResultTag::INVALID;
                break;
            default:
                break;
        }

        tag_ = o.tag_;
        o.invalidate();

        return *this;
    }

    [[nodiscard]] constexpr ResultTag& get_tag() { return tag_; }

    [[nodiscard]] constexpr E& get_error() & noexcept {
        assert_err(tag_);
        return *std::launder(reinterpret_cast<E*>(&error_));
    }

    [[nodiscard]] constexpr E&& get_error() && noexcept {
        assert_err(tag_);
        return std::move(*std::launder(reinterpret_cast<E*>(&error_)));
    }

    [[nodiscard]] constexpr const E& get_error() const& noexcept {
        assert_err(tag_);
        return *std::launder(reinterpret_cast<const E*>(&error_));
    }

    [[nodiscard]] constexpr const E&& get_error() const&& noexcept {
        assert_err(tag_);
        return std::move(*std::launder(reinterpret_cast<const E*>(&error_)));
    }

private:
    void invalidate() { tag_ = ResultTag::INVALID; }

    ResultTag tag_;
    union {
        type error_;
    };

    template <typename Rv, typename Ev>
    friend class result::Result;
};

template <typename E>
class ResultStorage<void, E, std::enable_if_t<!std::is_standard_layout_v<E> || !std::is_trivial_v<E>>> {
public:
    explicit constexpr ResultStorage(const Ok<void>&) noexcept : tag_(ResultTag::OK) {}

    explicit constexpr ResultStorage(const Ok<void>&&) noexcept : tag_(ResultTag::OK) {}

    explicit constexpr ResultStorage(const Err<E>& err) noexcept(std::is_nothrow_copy_constructible<E>())
        : tag_(ResultTag::ERR), error_(err.get_error()) {}
    explicit constexpr ResultStorage(const Err<E>&& err) noexcept(std::is_nothrow_move_constructible<E>())
        : tag_(ResultTag::ERR), error_(std::move(err.get_error())) {}

    ~ResultStorage() {
        switch (tag_) {
            case ResultTag::OK:
                break;
            case ResultTag::ERR:
                error_.~E();
                break;
            default:
                break;
        }
    }

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

private:
    ResultTag tag_;
    union {
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

template <>
struct Ok<void> {
    using value_type = void;

    [[nodiscard]] explicit constexpr Ok() noexcept {}

    constexpr Ok(Ok&&)    = default;
    constexpr Ok& operator=(Ok&&) = default;

    constexpr Ok(Ok const&) = default;
    constexpr Ok& operator=(Ok const&) = default;

    [[nodiscard]] constexpr bool operator==(const Ok<void>&) const { return true; }

    [[nodiscard]] constexpr bool operator!=(const Ok<void>&) const { return false; }

    [[nodiscard]] constexpr bool operator==(const Err<void>&) const { return false; }

    template <typename T>
    [[nodiscard]] constexpr bool operator!=(const Err<void>&) const {
        return true;
    }

private:
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

    constexpr Result(const Result& o) noexcept(std::is_nothrow_copy_constructible<Storage>()) : storage_(o.storage_) {}

    constexpr Result(Result&& o) noexcept(std::is_nothrow_move_constructible<Storage>()) : storage_(std::move(o.storage_)) {}

    constexpr Result& operator=(Result const& o) {
        storage_ = o.storage_;
        return *this;
    }

    constexpr Result& operator=(Result&& o) {
        storage_ = std::move(o.storage_);
        return *this;
    }

    [[nodiscard]] constexpr bool is_ok() { return storage_.tag_ == TagEnum::OK; }

    [[nodiscard]] constexpr bool is_err() { return storage_.tag_ == TagEnum::ERR; }

    template <typename U = E>
    [[nodiscard]] constexpr std::enable_if_t<!std::is_void_v<U>, U&> error() & noexcept {
        return storage_.get_error();
    }

    template <typename U = E>
    [[nodiscard]] constexpr std::enable_if_t<!std::is_void_v<U>, U&&> error() && noexcept {
        return std::move(storage_.get_error());
    }

    template <typename U = E>
    [[nodiscard]] constexpr std::enable_if_t<!std::is_void_v<U>, const U&> error() const& noexcept {
        return storage_.get_error();
    }

    template <typename U = E>
    [[nodiscard]] constexpr std::enable_if_t<!std::is_void_v<U>, const E&&> error() const&& noexcept {
        return std::move(storage_.get_error());
    }

    template <typename U = R>
    [[nodiscard]] constexpr std::enable_if_t<!std::is_void_v<U>, U&> result() & noexcept {
        return storage_.get_result();
    }

    template <typename U = R>
    [[nodiscard]] constexpr std::enable_if_t<!std::is_void_v<U>, U&&> result() && noexcept {
        return std::move(storage_.get_result());
    }

    template <typename U = R>
    [[nodiscard]] constexpr std::enable_if_t<!std::is_void_v<U>, const U&> result() const& noexcept {
        return storage_.get_result();
    }

    template <typename U = R>
    [[nodiscard]] constexpr std::enable_if_t<!std::is_void_v<U>, const U&&> result() const&& noexcept {
        return std::move(storage_.get_result());
    }

    // Helpful link about auto vs decltype(auto)
    // https://stackoverflow.com/questions/21369113/what-is-the-difference-between-auto-and-decltypeauto-when-returning-from-a-fun

    // and_then<R, Func>(Func&& f) -> Result<U, E>
    // where f(R r) -> Result<U, E>
    // and_then: takes a functor that takes the current result and returns a Result<U,E>
    // Examples:
    // Result<char, int> r{Ok{'a'}};
    // auto fin = r.and_then([](){
    //     return Result<std::string, int>{Ok{"a is the first letter in the Latin alphabet"}};
    // });

    // non-void
    template <typename F, typename X = R, typename = std::enable_if_t<!std::is_void_v<X>>>
    [[nodiscard]] constexpr auto and_then(
        F&& func) & -> Result<typename traits::invoke_result_t<F&&, X&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&, X&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&, X&&>::error_type, E>);

        return and_then_(storage_, std::forward<F>(func));
    }

    template <typename F, typename X = R, typename = std::enable_if_t<!std::is_void_v<X>>>
    [[nodiscard]] constexpr auto and_then(
        F&& func) && -> Result<typename traits::invoke_result_t<F&&, X&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&, X&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&, X&&>::error_type, E>);

        return and_then_(std::move(storage_), std::forward<F>(func));
    }

    template <typename F, typename X = R, typename = std::enable_if_t<!std::is_void_v<X>>>
    [[nodiscard]] constexpr auto and_then(
        F&& func) const& -> Result<typename traits::invoke_result_t<F&&, X&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&, X&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&, X&&>::error_type, E>);

        return and_then_(storage_, std::forward<F>(func));
    }

    template <typename F, typename X = R, typename = std::enable_if_t<!std::is_void_v<X>>>
    [[nodiscard]] constexpr auto and_then(
        F&& func) const&& -> Result<typename traits::invoke_result_t<F&&, X&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&, X&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&, X&&>::error_type, E>);

        return and_then_(std::move(storage_), std::forward<F>(func));
    }

    // void Result specialization
    template <typename F, typename X = R, typename = std::enable_if_t<std::is_void_v<X>>>
    [[nodiscard]] constexpr auto and_then(F&& func) & -> Result<typename traits::invoke_result_t<F&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&>::error_type, E>);

        return and_then_(storage_, std::forward<F>(func));
    }

    template <typename F, typename X = R, typename = std::enable_if_t<std::is_void_v<X>>>
    [[nodiscard]] constexpr auto and_then(
        F&& func) && -> Result<typename traits::invoke_result_t<F&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&>::error_type, E>);

        return and_then_(std::move(storage_), std::forward<F>(func));
    }

    template <typename F, typename X = R, typename = std::enable_if_t<std::is_void_v<X>>>
    [[nodiscard]] constexpr auto and_then(
        F&& func) const& -> Result<typename traits::invoke_result_t<F&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&>::error_type, E>);

        return and_then_(storage_, std::forward<F>(func));
    }

    template <typename F, typename X = R, typename = std::enable_if_t<std::is_void_v<X>>>
    [[nodiscard]] constexpr auto and_then(
        F&& func) const&& -> Result<typename traits::invoke_result_t<F&&>::result_type, E> {
        static_assert(traits::is_invocable_v<F&&>);
        static_assert(std::is_same_v<typename traits::invoke_result_t<F&&>::error_type, E>);

        return and_then_(std::move(storage_), std::forward<F>(func));
    }

    // map<R, Func>(Func&& f) -> Result<U, E>
    // where f(R r) -> U
    // map: takes a functor that takes the current result and returns a result of type U
    // Examples:
    // Result<char, int> r{Ok{'a'}};
    // auto fin = r.map([](){ return std::string{"a is the first letter in the Latin alphabet"}; });

    // non-void
    template <typename F, typename X = R, typename = std::enable_if_t<!std::is_void_v<X>>>
    [[nodiscard]] constexpr auto map(F&& func) & -> Result<traits::invoke_result_t<F&&, X&&>, E> {
        static_assert(traits::is_invocable_v<F&&, X&&>);
        static_assert(std::is_same_v<traits::first_argument_t<F>, X>);

        return map_(storage_, func);
    }

    template <typename F, typename X = R, typename = std::enable_if_t<!std::is_void_v<X>>>
    [[nodiscard]] constexpr auto map(F&& func) && -> Result<traits::invoke_result_t<F&&, X&&>, E> {
        static_assert(traits::is_invocable_v<F&&, X&&>);
        static_assert(std::is_same_v<traits::first_argument_t<F>, X>);

        return map_(std::move(storage_), func);
    }

    template <typename F, typename X = R, typename = std::enable_if_t<!std::is_void_v<X>>>
    [[nodiscard]] constexpr auto map(F&& func) const& -> Result<traits::invoke_result_t<F&&, X&&>, E> {
        static_assert(traits::is_invocable_v<F&&, X&&>);
        static_assert(std::is_same_v<traits::first_argument_t<F>, X>);

        return map_(storage_, func);
    }

    template <typename F, typename X = R, typename = std::enable_if_t<!std::is_void_v<X>>>
    [[nodiscard]] constexpr auto map(F&& func) const&& -> Result<traits::invoke_result_t<F&&, X&&>, E> {
        static_assert(traits::is_invocable_v<F&&, X&&>);
        static_assert(std::is_same_v<traits::first_argument_t<F>, X>);

        return map_(std::move(storage_), func);
    }

    // void Result specialization
    template <typename F, typename X = R, typename = std::enable_if_t<std::is_void_v<X>>>
    [[nodiscard]] constexpr auto map(F&& func) & -> Result<traits::invoke_result_t<F&&>, E> {
        static_assert(traits::is_invocable_v<F&&>);

        return map_(storage_, func);
    }

    template <typename F, typename X = R, typename = std::enable_if_t<std::is_void_v<X>>>
    [[nodiscard]] constexpr auto map(F&& func) && -> Result<traits::invoke_result_t<F&&>, E> {
        static_assert(traits::is_invocable_v<F&&>);

        return map_(std::move(storage_), func);
    }

    template <typename F, typename X = R, typename = std::enable_if_t<std::is_void_v<X>>>
    [[nodiscard]] constexpr auto map(F&& func) const& -> Result<traits::invoke_result_t<F&&>, E> {
        static_assert(traits::is_invocable_v<F&&>);

        return map_(storage_, func);
    }

    template <typename F, typename X = R, typename = std::enable_if_t<std::is_void_v<X>>>
    [[nodiscard]] constexpr auto map(F&& func) const&& -> Result<traits::invoke_result_t<F&&>, E> {
        static_assert(traits::is_invocable_v<F&&>);

        return map_(std::move(storage_), func);
    }

private:
    template <typename S, typename F, typename X = R, typename = std::enable_if_t<!std::is_void_v<X>>>
    [[nodiscard]] constexpr auto and_then_(S&& s, F&& func)
        -> Result<typename traits::invoke_result_t<F&&, X&&>::result_type, E> {
        if (is_ok()) {
            return func(std::forward<S>(s).get_result());
        } else {
            return Err<E>{std::forward<S>(s).get_error()};
        }
    }

    template <typename S, typename F, typename X = R, typename = std::enable_if_t<std::is_void_v<X>>>
    [[nodiscard]] constexpr auto and_then_(S&& s, F&& func)
        -> Result<typename traits::invoke_result_t<F&&>::result_type, E> {
        if (is_ok()) {
            return func();
        } else {
            return Err<E>{std::forward<S>(s).get_error()};
        }
    }

    template <typename S, typename F, typename X = R, typename = std::enable_if_t<!std::is_void_v<X>>>
    [[nodiscard]] constexpr auto map_(S&& s, F&& func) -> Result<traits::invoke_result_t<F&&, X&&>, E> {
        if (is_ok()) {
            return Ok<traits::invoke_result_t<F&&, X&&>>{func(std::forward<S>(s).get_result())};
        } else {
            return Err<E>{std::forward<S>(s).get_error()};
        }
    }

    template <typename S, typename F, typename X = R, typename = std::enable_if_t<std::is_void_v<X>>>
    [[nodiscard]] constexpr auto map_(S&& s, F&& func) -> Result<traits::invoke_result_t<F&&>, E> {
        if (is_ok()) {
            return Ok<traits::invoke_result_t<F&&>>{func()};
        } else {
            return Err<E>{std::forward<S>(s).get_error()};
        }
    }

    Storage storage_;
};

}  // namespace result
}  // namespace utils
}  // namespace cogle