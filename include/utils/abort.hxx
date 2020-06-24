#pragma once

#include <exception>


namespace cogle {
namespace utils {
namespace abort {

constexpr void abort() {
    std::terminate();
}

constexpr void assert(bool expect) {
    if (!__builtin_expect(expect, 0)) {
        abort();
    }
}


}  // namespace abort 
}  // namespace utils
}  // namespace cogle