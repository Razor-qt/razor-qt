#include "fixed_timeout.hpp"

#include <sys/time.h>


static uint64_t now_ms()
{
    timeval now;
    gettimeofday(&now, NULL);

    return (now.tv_sec * 1000 + now.tv_usec / 1000);
}

namespace DBus {

FixedTimeout::FixedTimeout(unsigned int interval, DefaultMainLoop *mainLoop)
    : DefaultTimeout(100, true, mainLoop)
{
    mExpired = now_ms() + interval;
}

bool FixedTimeout::isExpired() const
{
    return now_ms() >= mExpired;
}

}
