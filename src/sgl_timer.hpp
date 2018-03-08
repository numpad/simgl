#ifndef SGL_TIMER_HPP
#define SGL_TIMER_HPP

#include <iostream>
#include <string>
#include <functional>
#include <chrono>

namespace sgl {
	
	class timer {
		
	public:
		
		static double measure(std::function<void ()> func, std::string title = "");

	};

}

#endif

