#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/bernoulli.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>

#define MAX_TERMS_TO_EXPAND 999
// The above value defines how many terms to unflod in taylor expansion.

using f50d = boost::multiprecision::cpp_bin_float_50;  // 50 digit precision

namespace detail {
template <unsigned int start, unsigned int stop>
class taylor_expand_impl {
  static_assert(start < stop);

 public:
  template <bool add_terms, typename Callable>
  constexpr auto operator()(Callable&& c) {
    unsigned int term = start;

    static_assert(std::is_invocable<Callable, unsigned int>::value);
    using return_t = typename std::invoke_result<Callable, unsigned int>::type;
    static_assert(std::is_constructible<return_t, int>::value);

    return_t result{0};

    std::vector<return_t> convergence_history;

    while (true) {
      if (add_terms) {
        result += c(term);
        convergence_history.emplace_back(result);
      } else {
        result -= c(term);
        convergence_history.emplace_back(result);
      }

      term++;
      if (term == stop) {
        break;
      }

      else if (convergence_history.size() > 1) {
        if (has_convered(convergence_history.back(),
                         *(convergence_history.end() - 2)))
          break;
      }
    }

    return std::make_pair(result, convergence_history);
  }

 private:
  template <class T>
  constexpr inline bool has_convered(T current, T last) {
    if (current == last) {
      std::cout << "\tExpansion has converged :  " << std::fixed
                << std::setprecision(50) << current << "\n";
      return true;
    }
    return false;
  }
};
}  // namespace detail

/**
 * @brief Taylor Expansion object which starts the expansion and returns the
 * result as well as the history of the converge or diverge upto
 * `MAX_EXPANSION_DEPTH`
 *
 * @tparam termStart The start term index in the Expansion
 * @tparam termStop The stop term index in the Expansion
 */
template <unsigned int termStart, unsigned int termStop>
class TaylorExpansion {
 public:
  /**
   * @brief Adds all terms that are returned from the Callable
   *
   * @tparam Callable The callable which generates the nth term if passed
   * unsigned int n
   * @param c The object of the callable, can be a lambda but must return value
   * non void type
   * @return std::pair, the result of expansion and history of convergence is
   * returned.
   */
  template <class Callable>
  auto expand_by_adding(Callable&& c) {
    return taylor.template operator()<true>(std::forward<Callable>(c));
  }
  /**
   * @brief Subtracts all terms that are returned from the Callable
   *
   * @tparam Callable The callable which generates the nth term if passed
   * unsigned int n
   * @param c The object of the callable, can be a lambda but must return value
   * non void type
   * @return std::pair, the result of expansion and history of convergence is
   * returned.
   */
  template <class Callable>
  auto expand_by_subtracting(Callable&& c) {
    return taylor.template operator()<false>(std::forward<Callable>(c));
  }
  /**
   * @brief Adds all terms returned by the Callable and returns only the result
   * of the expansion
   *
   * @tparam Callable The callable which generates the nth term if passed
   * unsigned int n
   * @param The object of the callable, can be a lambda but must return value
   * non void type
   * @return auto, the returned approximation of the function.
   */
  template <class Callable>
  auto approximate_by_adding(Callable&& c) {
    return expand_by_adding(std::forward<Callable>(c)).first;
  }
  /**
   * @brief Subtracts all terms returned by the Callable and returns only the result
   * of the expansion
   *
   * @tparam Callable The callable which generates the nth term if passed
   * unsigned int n
   * @param The object of the callable, can be a lambda but must return value
   * non void type
   * @return auto, the returned approximation of the function.
   */
  template <class Callable>
  auto approximate_by_subtracting(Callable&& c) {
    return expand_by_subtracting(std::forward<Callable>(c)).first
  }

 private:
  detail::taylor_expand_impl<termStart, termStop> taylor;
};

TaylorExpansion<0, MAX_TERMS_TO_EXPAND> taylor;

template <class Callable>
void eval_and_print(int number, std::string title, Callable&& call, f50d ans) {
  std::cout << number << ". " << title << "\n";

  auto result = taylor.expand_by_adding(std::forward<Callable>(call));

  if (result.second.size() != MAX_TERMS_TO_EXPAND)
    std::cout << "\tTotal terms expanded before convergence : "
              << result.second.size() << "\n";
  else
    std::cout << "\tExpansion did not converged, but series expansion limit "
              << MAX_TERMS_TO_EXPAND << " reached \n";

  std::cout << "\tExpansion resulted value : " << std::fixed
            << std::setprecision(50) << result.first << "\n";

  std::cout << "\tResult by multiprecision : " << ans << "\n";

  std::cout << "\tDifference of expansions : "
            << boost::multiprecision::abs(ans - result.first) << "\n\n";
}

int main() {
  std::cout << "**************************** STARTED EXPANSION SUIT "
               "*******************************\n";

  std::cout << "*********************"
            << " MAXIMUM EXPANSION OF SERIES WILL BE " << MAX_TERMS_TO_EXPAND
            << " **********************\n\n";

  eval_and_print(
      1, "Taylor Expansion of sin(pie/4) ",
      [](unsigned int n) {
        // Sin(x) = for i 0 to infinity : ((-1)^i/(2i+1)!)* x^(2n+1)

        f50d x = boost::math::constants::quarter_pi<f50d>();
        f50d factorial = boost::math::factorial<f50d>(2 * n + 1);
        int num = 1;
        if (n & 1) num = -1;
        f50d multiplier = boost::multiprecision::pow(x, 2 * n + 1);
        return (f50d(num) / factorial) * multiplier;
      },
      boost::multiprecision::sin(boost::math::constants::quarter_pi<f50d>()));

  eval_and_print(
      2, "Taylor Expansion of cos(pie/6) ",
      [](unsigned int n) {
        // Cos(x) = for i 0 to infinity : ((-1)^i/(2i)!)* x^(2n)

        f50d x = boost::math::constants::sixth_pi<f50d>();
        f50d factorial = boost::math::factorial<f50d>(2 * n);
        int num = 1;
        if (n & 1) num = -1;
        f50d multiplier = boost::multiprecision::pow(x, 2 * n);
        return (f50d(num) / factorial) * multiplier;
      },
      boost::multiprecision::cos(boost::math::constants::sixth_pi<f50d>()));

  eval_and_print(
      3, "Taylor Expansion of tan(pie/3) ",
      [](unsigned int n) {
        n++;  // because n should starts from 1 for this series

        // clang-format off
        // tan(x) = for i 1 to infinity : ((B2n * (-4)^n * (1 - 4^n))/2n!)*x^(2n-1), Bk is kth Bernauli number
        // clang-format on

        f50d x = boost::math::constants::third_pi<f50d>();
        f50d B2n = boost::math::bernoulli_b2n<f50d>(n);
        f50d four_pow_n = boost::multiprecision::pow(f50d(4), n);
        int sign = 1;
        if (n & 1) sign = -1;
        f50d factorial = boost::math::factorial<f50d>(2 * n);
        f50d multiplier = boost::multiprecision::pow(x, 2 * n - 1);
        return ((B2n) * (four_pow_n)*f50d(sign) * (f50d(1) - four_pow_n) /
                factorial) *
               multiplier;
      },
      boost::multiprecision::tan(boost::math::constants::third_pi<f50d>()));

  eval_and_print(
      4, "Taylor Expansion of cosh(5*pie/6) ",
      [](unsigned int n) {
        // Cosh(x) = for i 0 to infinity : (x^(2n)/(2i)!)

        f50d x = 5 * boost::math::constants::sixth_pi<f50d>();
        f50d factorial = boost::math::factorial<f50d>(2 * n);
        f50d multiplier = boost::multiprecision::pow(x, 2 * n);
        return (multiplier / factorial);
      },
      boost::multiprecision::cosh(5 *
                                  boost::math::constants::sixth_pi<f50d>()));

  eval_and_print(
      5, "Taylor Expansion of asinh(pie/4) ",
      [](unsigned int n) {
        // Sin(x) = for i 0 to infinity : ((-1)^i/(2i+1)!)* x^(2n+1)

        f50d x{0.3};
        f50d factorial = boost::math::factorial<f50d>(2 * n);
        f50d n_factorial_square =
            boost::multiprecision::pow(boost::math::factorial<f50d>(n), 2);
        f50d four_n_power = boost::multiprecision::pow(f50d{4}, n);
        int num = 1;
        if (n & 1) num = -1;
        f50d multiplier = boost::multiprecision::pow(x, 2 * n + 1);
        return ((num * factorial) /
                ((four_n_power * n_factorial_square) * (2 * n + 1))) *
               multiplier;
      },
      boost::multiprecision::asinh(f50d{0.3}));

  eval_and_print(
      6, "Taylor Expansion of ln(1.2131234514524364132) ",
      [](unsigned int n) {
        n++;

        // ln(1+x) = for i 1 to infinity : ((-1)^(n+1))* x^n/n

        f50d x{0.2131234514524364132};
        f50d multiplier = boost::multiprecision::pow(x, n);
        int sign = 1;
        if ((n + 1) & 1) sign = -1;
        return sign * ((multiplier) / n);
      },
      boost::multiprecision::log(f50d{1 + 0.2131234514524364132}));

  eval_and_print(
      7, "McLauren Expansion of 1/(1-x) at 0.73245154367542122134",
      [](unsigned int n) {
        // 1/(1-x) : for i from 0 to infinity : x^i

        return boost::multiprecision::pow(f50d{0.73245154367542122134}, n);
      },
      f50d{1} / (1 - f50d{0.73245154367542122134}));

  eval_and_print(
      8, "Binomial Expanion of (1+x)^(-1/3) at 0.6532457865348754",
      [](unsigned int n) {
        // clang-format off
        // (1+x)^(-1/3) : sum for i from 0 to infinity : (product for k from 1 to infinity : (-1/3 - k + 1 ) / k) * x^
        // clang-format on

        f50d x{0.6532457865348754};
        f50d multiplier = boost::multiprecision::pow(x, n);
        f50d alpha{-0.33333333333333333};
        f50d coefficient = 1;
        for (int t = 1; t <= n; t++) {
          coefficient *= (alpha - f50d{t - 1}) / f50d{t};
        }
        return coefficient * multiplier;
      },
      1 / boost::multiprecision::cbrt(f50d{1.6532457865348754}));

  eval_and_print(
      9, "Approximating exponent",
      [](unsigned int n) {
        // e = for i from 0 to infinity : 1/i!
        return 1 / boost::math::factorial<f50d>(n);
      },
      boost::math::constants::e<f50d>());

  return 0;
}