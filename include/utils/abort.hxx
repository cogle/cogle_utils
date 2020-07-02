#pragma once

#include <exception>
#include <iostream>

namespace cogle {
namespace utils {
namespace abort {

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