#pragma once

#include <exception>
#include <iostream>
#include <utils/compatibility.hxx>

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
    LIKELY_IF(expect) { return; }
    abort(std::forward<Args>(args)...);
}

}  // namespace abort
}  // namespace utils
}  // namespace cogle