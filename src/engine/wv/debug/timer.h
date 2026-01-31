#pragma once

#include <chrono>

namespace wv {

class Timer
{
public:
	Timer() {
		reset();
	}

	// in seconds
	double elapsed() const {  
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = now - m_start;
		return elapsed.count();
	}

	void reset() {
		m_start = std::chrono::steady_clock::now();
	}

private:
	std::chrono::steady_clock::time_point m_start{};
};

}