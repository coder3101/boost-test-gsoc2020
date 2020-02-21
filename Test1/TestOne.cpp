#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <chrono>
#include <iostream>
#include <numeric>
#include <optional>
#include <random>
#include <utility>

#define USE_1028_UINT

// If you uncomment it, we will use 2048 bit unsigned.
// But that's insanity and that number is 7 times more than the number of atoms
// in the universe.

#if defined(USE_1028_UINT)

using boost::multiprecision::checked_uint1024_t;
using ump = checked_uint1024_t;  // We always avoid overflow

#else
namespace bmp = boost::multiprecision;

using checked_uint2048_t =
    bmp::number<bmp::cpp_int_backend<2048, 2048, bmp::unsigned_magnitude,
                                     bmp::checked, void> >;
using ump = checked_uint2048_t; // ump stands for "unsigned multiprecision"

#endif

/**
 * @brief `StopWatch` creates a stopwatch which starts as soon as it is created.
 * At any time user can call now() method to get time elapsed since creation in
 * **micro-seconds**.
 *
 */
class StopWatch {
 public:
  StopWatch() { start_ = std::chrono::high_resolution_clock::now(); }
  /**
   * @brief Returns time since creation of `this` object in microseconds
   *
   * @note now() function can be called multiple times, each time it returns
   * the duration since creation of object.
   *
   * @return unsigned long int
   */
  unsigned long int now() const {
    using namespace std::chrono;
    auto end = high_resolution_clock::now();
    auto time_diff = end - start_;
    return duration_cast<microseconds>(time_diff).count();
  }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

/**
 * @brief This function prompts on stdin if user would like to provide the 1024
 * bit unsigned number
 *
 * @return std::optional<u1024>, has value if user entered otherwise optional
 * does not have any value
 */
std::optional<ump> ask_user() {
  std::string res;

#if defined(USE_1028_UINT)
  std::cout << "Would you like to enter 1024 bit number yourself?\n";
#else
  std::cout << "Would you like to enter 2048 bit number yourself?\n";
#endif

  std::cin >> res;

  ump number;
  if (res[0] == 'Y' || res[0] == 'y') {
    std::cout << "Enter the number : ";
    std::cin >> number;
    return number;
  }
  return {};
}

/**
 * @brief This function generates a random 1024/2048 bit usigned number. It uses
 * messier twister engine to uniformly sample interger distribution.
 *
 * @note The number is always in range [2^1000 - 2^1024/2048)
 *
 * @return ump, an unsigned 1000-1024/2048 bit number
 */
ump generate_random_multiprecision() {
  std::random_device rd;
  boost::random::mt19937 messier_engine(rd());

  ump lower_limit = ump(1) << 1000;                   // 2 ^ 1000
  ump upper_limit = std::numeric_limits<ump>::max();  // max,
  boost::random::uniform_int_distribution<ump> uid(lower_limit, upper_limit);
  return uid(messier_engine);
}

/**
 * @brief Computes the time required and value of the integral sqrt of an 1024/2048
 * bit unsigned number using boost::multiprecision::sqrt.
 *
 * @param num, a 1024/2048 bit unsigned number whose sqrt is required
 *
 * @return std::pair<ump, unsigned long int>, integral square root and time
 * taken in microsecond
 */
std::pair<ump, unsigned long int> perform_sqrt_by_multiprecision(ump num) {
  StopWatch watch;
  auto result = boost::multiprecision::sqrt(num);
  auto time = watch.now();
  return std::make_pair(result, time);
}

/**
 * @brief Computes the time required and value of the integral sqrt of an 1024/2048
 * bit unsigned number using babylonian method.
 *
 * @param num, a 1024/2048 bit unsigned number whose sqrt is required
 *
 * @return std::pair<ump, unsigned long int>, integral square root and time
 * taken in microsecond
 */
std::pair<ump, unsigned long int> perform_sqrt_by_babylonian(ump num) {
  StopWatch watch;
  ump approx = num;
  ump y = 1;
  while (approx > y) {
    approx = (approx + y) / 2;
    y = num / approx;
  }
  return std::make_pair(approx, watch.now());
}

int main() {
  ump number = ask_user().value_or(generate_random_multiprecision());

  auto response = perform_sqrt_by_multiprecision(number);
  auto response2 = perform_sqrt_by_babylonian(number);

  std::cout << "Your number is : " << number << std::endl;
  std::cout << "Your number in hex : " << std::hex << std::showbase << number
            << std::endl;

  std::cout << "\nSquare root by boost::multiprecision::sqrt() : " << std::dec
            << response.first << std::endl;
  std::cout << "Square root in hex : " << std::hex << std::showbase
            << response.first << std::endl;
  std::cout << "Computation took : " << std::dec << response.second << "us"
            << std::endl;

  std::cout << "\nSquare root by babylonian : " << response2.first << std::endl;
  std::cout << "Square root in hex : " << std::hex << std::showbase
            << response2.first << std::endl;
  std::cout << "Computation took : " << std::dec << response2.second << "us"
            << std::endl;

  std::cout << "\nDid both above reported same sqrt? : "
            << ((response.first == response2.first) ? "YES" : "NO")
            << std::endl;

  return 0;
}