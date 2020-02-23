## [Programming competency test 2](https://github.com/boostorg/boost/wiki/Google-Summer-of-Code%3A-2020#programming-competency-test)

#### Statement
Use the existing Boost.Multiprecision `cpp_bin_float` type to perform an extended-precision (say 50 decimal digits of precision) floating-point calculation of any function you like. This could be a linear combination of elementary transcendental functions, a numerical derivative, a numerical integration or similar. Show your mastery of looping and handling iterative calculations. In this sense, it would be good to select a computation that has enough depth to require an iterative loop such as a `for`-loop. A Taylor series approximation or another kind of asymptotic approximation of your choice will be a good choice.

#### Machine configuration

- Architecture : Intel x64
- Boost version : 1.72.0
- Compiler : MSVC 19.23

#### Solution

Taylor Series can approximate most of the functions and hence, I had chosen it over other choice, I wanted to approximate as many functions that I could each of all types, from `trignometric` to `hyperbolic` to `logarithmic` functions at some point. All the approximation functions were ran with `50` digit of precision. I have tried my best to demonstrate the template programming skills and code reusability. Later those approximation were used to see how close we got to `boost::multiprecision::<func>` and `boost::math::<func>` values. 

I have approximated following functions at some point:

| Function       | at                       | Relative Error in approximation                      |
| -------------- | ------------------------ | ---------------------------------------------------- |
| `sin`          | `pie/4`                  | `0`                                                  |
| `cos`          | `pie/6`                  | `0`                                                  |
| `tan`          | `pie/3`                  | `0`                                                  |
| `cosh`         | `5*pie/6`                | `0`                                                  |
| `asinh`        | `pie/4`                  | `0`                                                  |
| `Natual log`   | `1.2131234514524364132`  | `00000000000000006863829624857550718573583470386359` |
| `1/(1-x)`      | `0.73245154367542122134` | `0`                                                  |
| `(1+x)^(-1/3)` | `0.6532457865348754`     | `00000000000000001106368628320741610847428318606341` |
| `e^x`          | `1`                      | `0`                                                  |



#### Implementation Details

I created a general class `TaylorExpansion<unsigned start, unsigned stop>` that in general can expand any function starting from term `start` to `end`. Later, In its object you could call `expand_by_adding(Callable c)` and suppose Callable return type is `P`, this function will return `std::pair<P, std::vector<P>>` , In first you have the approximation of the function represented by Callable and In second you have Value of Approximation at each point during expansion. So that you can check, how your function converged.

I have used `static_asserts` that places following constraints on `Callable`

- Callable must return a value constructible from `int`
- It should be invokable by `unsigned int` 

> NOTE : The above class will Early Converge, if during expansion we have converged to 50 precision because there is no need to expand, if we can't get more precision.

#### Usage

This program does not interacts over `stdout`, As soon as you start it should start printing all the various approximation. However, here is how to use the library to approximate value of `e`.

```
e = 1/factorial(0) + 1/factorial(1) + 1/factorial(2) + .... (upto infinity)
```

We have to prepare a function that when given term number `t`, returns the value of that `term`.

```c++
// Remove main and maybe rename
#include "TestTwo.hpp"

double e_term(unsigned int n)
{
    return 1.0 / factorial(n); // suppose we have this function defined
}

int main()
{
    TaylorExpansion<0, 30> taylor; // expands upto 30 terms maximum
    std::cout << taylor.approximate_by_adding(e_term) << endl;
    return 0;
}

```



#### Benchmarking on Convergence

I decided to do some experiment with the convergence data I had about expansion of all those above mentioned functions and decided to plot the number of terms required before convergence different functions.  I ran Expansion up to `999` terms, If the expansion did not converged then we returned.

![img](https://i.imgur.com/oLMUzSb.png)

> Those with 1000 did not converged at all. These values may also vary on the basis of at which point they are evaluated. I have generated and tested them on random valid points

