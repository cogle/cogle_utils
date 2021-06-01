#pragma once

namespace cogle {
namespace utils {

#if __cplusplus >= 202002L
#define LIKELY_IF(x) \
    if (x) [[likely]]
#define UNLIKELY_IF(x) \
    if (x) [[unlikely]]
#else
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define LIKELY_IF(x)   if (LIKELY(x))
#define UNLIKELY_IF(x) if (UNLIKELY(x))
#endif

}  // namespace utils
}  // namespace cogle