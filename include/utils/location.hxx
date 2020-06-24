#pragma once

#include <iostream>

namespace cogle {
namespace utils {
namespace location {

namespace detail {
static constexpr auto DEFAULT_LOC_CHAR_ARR_VAL = "unknown";
}

// https://en.cppreference.com/w/cpp/utility/source_location
// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/experimental/source_location
struct SourceLocation {
public:
    constexpr SourceLocation() noexcept
        : file_(detail::DEFAULT_LOC_CHAR_ARR_VAL), func_(detail::DEFAULT_LOC_CHAR_ARR_VAL), line_(0), col_(0) {}
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

    constexpr int line() const noexcept { return line_; }
    constexpr int column() const noexcept { return col_; }

    constexpr const char* file_name() const noexcept { return file_; }
    constexpr const char* function_name() const noexcept { return func_; }

    friend std::ostream& operator<<(std::ostream& os, const SourceLocation& sl) {
        os << "File: " << sl.file_ << "\tFunc: " << sl.func_ << "\tLine: " << sl.line_ << "\tCol: " << sl.col_;
        return os;
    }

private:
    const char* file_;
    const char* func_;
    int line_;
    int col_;
};
}  // namespace location
}  // namespace utils
}  // namespace cogle