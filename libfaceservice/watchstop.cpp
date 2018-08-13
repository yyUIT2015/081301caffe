#include "watchstop.h"


WatchStop::WatchStop(void)
{
}


WatchStop::~WatchStop(void)
{
}

void WatchStop::start()
{
	
    /*QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&begin);
	
	begin_ = GetTickCount();*/

	begin = bpt::microsec_clock::local_time();

    
}

unsigned long WatchStop::time_elapsed_us()
{
	/*QueryPerformanceCounter(&end);
    double interval = (double) (end.QuadPart - begin.QuadPart) / frequency.QuadPart;//return seconds unit
	return interval * 1000000;*/

	
    stop = bpt::microsec_clock::local_time();

    bpt::time_duration dur = stop - begin;

	return dur.total_microseconds();

}
unsigned long WatchStop::time_elapsed_ms()
{
	/*QueryPerformanceCounter(&end);
    double interval = (double) (end.QuadPart - begin.QuadPart) / frequency.QuadPart;//return second unit
	return interval * 1000;*/

	
    stop = bpt::microsec_clock::local_time();

    bpt::time_duration dur = stop - begin;

	return dur.total_milliseconds();

}

