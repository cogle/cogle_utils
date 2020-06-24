#pragma once

#include <exception>
#include <iostream>

namespace cogle {
namespace utils {
namespace abort {

// https://en.cppreference.com/w/cpp/utility/source_location
// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/experimental/source_location
struct SourceLocation {
public:
    constexpr SourceLocation() noexcept : file_("unknown"), func_("unknown"), line_(0), col_(0) {}
    ~SourceLocation() = default;

    static constexpr SourceLocation current(const char* file = __builtin_FILE(),
                                            const char* func = __builtin_FUNCTION(), int line = __builtin_LINE(),
                                            int col = 0) noexcept {
        SourceLocation sl{};

        sl.file_ = file;
        sl.func_ = func;
        sl.line_ = line;
        sl.col_  = col;

        return sl;
    }

    constexpr unsigned int line() const noexcept { return line_; }
    constexpr unsigned int column() const noexcept { return col_; }

    constexpr const char* file_name() const noexcept { return file_; }
    constexpr const char* function_name() const noexcept { return func_; }

    friend std::ostream& operator<<(std::ostream& os, const SourceLocation& sl) {
        os << "File: " << sl.file_ << "\tFunc: " << sl.func_ << "\tLine: " << sl.line_ << "\tCol: " << sl.col_;
        return os;
    }

private:
    const char* file_;
    const char* func_;
    unsigned int line_;
    unsigned int col_;
};

template <typename... Args>
[[noreturn]] void abort(Args&&... args) {
    ((std::cout << args << " "), ...) << std::endl << std::flush;
    std::terminate();
}

template <typename... Args>
constexpr void cogle_assert(bool expect, Args&&... args) {
    if (!__builtin_expect(expect, 0)) {
        abort(std::forward<Args>(args)...);
    }
}

}  // namespace abort
}  // namespace utils
}  // namespace cogle