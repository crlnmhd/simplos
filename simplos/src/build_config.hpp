#ifndef BUILD_CONFIG_HPP_
#define BUILD_CONFIG_HPP_

// clang-format off
#if defined(__clang__)
_Pragma("clang diagnostic ignored \"-Wlanguage-extension-token\"")
#endif // defined (__clang__)

#define DO_PRAGMA_(x) _Pragma(#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)

#if defined(__clang__)

#define CLANG_IGNORE_BEGIN(warning)                             \
  _Pragma("clang diagnostic push")                              \
  _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"")     \
  DO_PRAGMA(clang diagnostic ignored warning)

#define CLANG_IGNORE_END() _Pragma("clang diagnostic pop")
#define GCC_IGNORE_BEGIN(...)
#define GCC_IGNORE_END()

#elif defined(__GNUC__)

#define GCC_IGNORE_BEGIN(warning)                               \
  _Pragma("GCC diagnostic push")                                \
  _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"");      \
  DO_PRAGMA(GCC diagnostic ignored warning)

#define GCC_IGNORE_END() _Pragma("GCC diagnostic pop")

#define CLANG_IGNORE_BEGIN(...)
#define CLANG_IGNORE_END(...)

// clang-format on
#endif  // __GNUC__

#endif  // BUILD_CONFIG_HPP_
