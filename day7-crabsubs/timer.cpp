#include "include/timer.h"

using namespace dmac;
using namespace std::chrono;

dmac::Timer::Timer()
{
	startTime_ = steady_clock::now();
	stopTime_ = steady_clock::now();
}

float
dmac::Timer::timeTakenMilli()
{
	auto timeTaken = stopTime_ - startTime_;
	auto timeTakenMicro = duration_cast<microseconds>(timeTaken);
	

	return timeTakenMicro.count() / 1000.0f;;
}

float
dmac::Timer::timeTakenSeconds()
{
	auto timeTaken = stopTime_ - startTime_;
	auto timeTakenMicro = duration_cast<microseconds>(timeTaken);
	

	return timeTakenMicro.count() / 1'000'000.0f;;
}

steady_clock::time_point 
dmac::Timer::start()
{
	startTime_ = steady_clock::now();
	return startTime_;
}

steady_clock::time_point
dmac::Timer::stop()
{
	stopTime_ = steady_clock::now();
	return steady_clock::now();
}
