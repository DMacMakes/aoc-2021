#pragma once
#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>

namespace dmac
{
	using namespace std::chrono;
	class Timer {

	public:
		Timer();
		float timeTakenMilli();
    float timeTakenSeconds();
		steady_clock::time_point start();
		steady_clock::time_point stop();

	private:
		steady_clock::time_point startTime_;
		steady_clock::time_point stopTime_;

	};  // class timer

}

#endif 