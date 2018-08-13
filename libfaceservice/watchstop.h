#pragma once
#include <boost/date_time/posix_time/posix_time.hpp>

namespace bpt = boost::posix_time;
class WatchStop
{
public:
	WatchStop(void);
	~WatchStop(void);
	void start();
	unsigned long time_elapsed_us();
	unsigned long time_elapsed_ms();
private:
	bpt::ptime begin, stop;

};

