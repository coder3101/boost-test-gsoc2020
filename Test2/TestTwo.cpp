#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/math/constants/constants.hpp>
#include <iostream>

using f50d = boost::multiprecision::cpp_bin_float_50;


int main()
{
	f50d num = boost::math::constants::half_pi<f50d>();
	f50d ans = boost::multiprecision::sin(num);

	std::cout << ans << std::endl;

	// Approximate sin function using taylor expansion

	for (int t = 1; t < 50; t++)
	{
		f50d calc;
		int power = 1;
		bool toggle = true;
		for (int term = 0; term < t; term++)
		{
			if (toggle)
				calc += boost::multiprecision::pow(num, power) / boost::math::factorial<f50d>(power);
			else
				calc -= boost::multiprecision::pow(num, power) / boost::math::factorial<f50d>(power);
			toggle = !toggle;
			power += 2;
		}
		std::cout << "Difference(n=" << t << ") : " << boost::multiprecision::abs(calc - ans) << "\n";
	}
	return 0;
}