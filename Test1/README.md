## [Programming competency test 1](https://github.com/boostorg/boost/wiki/Google-Summer-of-Code%3A-2020#programming-competency-test)

#### Statement
Using latest Boost.Multiprecision, calculate and print the **real-valued, positive integral square 
root** of an unsigned integer having 1024 bits or more. Print the result to std::cout in both decimal as well as hexadecimal representation.

#### Machine configuration

- Architecture : Intel x64
- Boost version : 1.72.0
- Compiler : MSVC 19.23

#### Solution

In order to perform various mathematical operations like square root, provided as free
functions. I used `boost::multiprecision::sqrt`. As far as the output and format
was concerned I used `std::hex` to print numbers in hex format to `stdout` and `std::showbase`
for the sweet `0x` in front.

The source also provides a configuration macro named `USE_1024_UINT`, which if defined forces
all operation to take place on 1024 bit unsigned checked integer. If not defined, we use 2048 bit.

Instead of fixing a 1024 or 2048 bit number always, I have used `boost::random` to sample uniform random distribution which is seeded with a random device, the code also has been well documented with doxygen type comment blocks.

To measure performance, I have also added small `StopWatch` class that calculates the time taken for various functions to run.
This is used for benchmarking as described below.

#### Usage

There isn't much to usage, when the program is executed it asks you if you would like to Enter a 1024 bit number yourself? If you answer beings with `Y` or `y`, It will ask the number otherwise it will generate a new random number of in range `[2^1000, 2^1024 or 2^2048)` and them compute the square root of the number and print its `hex` and `dec` with time taken to compute the square root. 

#### Benchmarking

The babyloanian method for computing the square-root of the number was in general always **20 times slower**than its counterpart `boost::multiprecision::sqrt`. I haven't looked at multiprecision implementation yet, but I have found other better approximating algorithm 
that work better than what I have used above. Let's not *"re-invent the wheel?"* `multiprecision` is always way fast.

Results of average of 30 runs of sqrt computation on random 1024 bit unsigned is depicted below:

![ img ](https://i.imgur.com/3rruMNB.png)

> Not acceptable, maybe In future I will try to improve this algorithm.

#### Objectives and thoughts

The main objective of this part of test in my opinion was to understand how well
multiprecision library interacts with the standard libraries. The formatting of
the output based on different numeric systems (like `std::hex`, `std::oct`, `std::dec`) were
possible with `boost::multiprecision::cpp_int`, standard numeric traits worked flawlessly and felt like I wasn't using any external numeric data-type.