#ifndef SSTD_HPP_
#define SSTD_HPP_

/* Substibute standard library
 *
 * Helper functions to substitute parts of the missing C++ standard library.
 *
 * sstd::enable_if
 * */

namespace sstd {
template <class T, T v>
struct integral_constant {
  static constexpr T value = v;
  using value_type = T;
  using type = integral_constant;
  constexpr operator value_type() const noexcept { return value; }
  constexpr value_type operator()() const noexcept { return value; }
};

struct false_type : integral_constant<bool, false> {};
struct true_type : integral_constant<bool, true> {};

template <typename T, typename V>
struct is_same : false_type {};

template <class T>
struct is_same<T, T> : true_type {};

template <bool B, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> {
  typedef T type;
};

template <bool B, class T = void>
using enable_if_t = typename enable_if<B, T>::type;

}  // namespace sstd

#endif  // SSTD_HPP_
