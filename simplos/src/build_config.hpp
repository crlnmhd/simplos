#ifndef BUILD_CONFIG_HPP_
#define BUILD_CONFIG_HPP_

// clang-format off
#if defined(__clang__)
_Pragma("clang diagnostic ignored \"-Wlanguage-extension-token\"")
#endif // defined (__clang__)

//clang-format on
#define DO_PRAGMA_(x) _Pragma(#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)
//clang-format off
#if defined(__clang__)
#define CLANG_IGNORE_BEGIN(warning)                             \
  _Pragma("clang diagnostic push")                              \
      _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
          DO_PRAGMA(clang diagnostic ignored warning)
#define CLANG_IGNORE_END() _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define GCC_IGNORE_BEGIN(warning)                             \
  _Pragma("GCC diagnostic push")                              \
      _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"") \
          DO_PRAGMA(GCC diagnostic ignored warning)
#define GCC_IGNORE_END() _Pragma("GCC diagnostic pop")
#endif  // __GNUC__

#endif  // BUILD_CONFIG_HPP_
