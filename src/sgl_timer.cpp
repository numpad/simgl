#include "sgl_timer.hpp"


double sgl::timer::measure(std::function<void ()> func, std::string description)
{
	const auto start = std::chrono::high_resolution_clock::now();
	func();
	const auto end = std::chrono::high_resolution_clock::now();
	
	const std::chrono::duration<double, std::milli> diff = end - start;
	const double passed_time = diff.count();
	if (description[0])
		std::cout << description << ": " << passed_time << "ms" << std::endl;
	
	return passed_time;
}


