#ifndef TIMESPAN_H__
#define TIMESPAN_H__


#define SECS_PER_DAY	86400.0
#define MSEC_PER_DAY    86400000.0


class TimeSpan
{
private:
    double seconds;
public:
    TimeSpan(double seconds)
        : seconds(seconds)
    {
    }
    TimeSpan()
        : seconds(0)
    {
    }

    double getTotalSeconds() { return this->seconds; }
    void setTotalSeconds(double secs) { this->seconds = secs; }
};


#endif//TIMESPAN_H__

