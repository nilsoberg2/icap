
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "datetime.h"

#define UCHAR(x) (((x) >= 'a' && (x) <= 'z') ? ((x)&~32) : (x))

static const char* MonthTxt[] =
    {"JAN", "FEB", "MAR", "APR",
     "MAY", "JUN", "JUL", "AUG",
     "SEP", "OCT", "NOV", "DEC"};
static const int DaysPerMonth[2][12] =      // days per month
    {{31, 28, 31, 30, 31, 30,               // normal years
      31, 31, 30, 31, 30, 31},
     {31, 29, 31, 30, 31, 30,               // leap years
      31, 31, 30, 31, 30, 31}};
static const int DateDelta = 693594;        // days since 01/01/00
static const double SecsPerDay = 86400.;    // seconds per day

double EPSILON = 1e-6;

int  findMonth(const char* month);
double encodeDate(int year, int month, int day);
double encodeTime(int hour, int minute, int second);
void timeToStr(double time, char* s);
void dateToStr(double date, char* s, Format::DateFormat format);
void decodeTime(double time, int* h, int* m, int* s);
void decodeDate(double date, int* year, int* month, int* day);


class DateTime::Impl
{
public:
    double datetime;
};


DateTime::operator double()
{
    return impl->datetime;
}

DateTime::operator double() const
{
    return impl->datetime;
}

DateTime& DateTime::operator /=(const double& value)
{
    impl->datetime /= value;
    return *this;
}

DateTime& DateTime::operator +=(const double& value)
{
    impl->datetime += value;
    return *this;
}

DateTime& DateTime::operator +=(const DateTime& value)
{
    impl->datetime += value.impl->datetime;
    return *this;
}

DateTime DateTime::operator +(const DateTime& value) const
{
    DateTime dt;
    dt.impl->datetime = this->impl->datetime + value.impl->datetime;
    return dt;
}

TimeSpan DateTime::operator -(const DateTime& value) const
{
    return TimeSpan((this->impl->datetime - value.impl->datetime) * SECS_PER_DAY);
}


DateTime& DateTime::operator =(const DateTime& value)
{
    this->impl->datetime = value.impl->datetime;
    return *this;
}

DateTime& DateTime::operator =(const double& value)
{
    this->impl->datetime = value;
    return *this;
}

bool DateTime::operator ==(const DateTime& value) const
{
    return fabs(value.impl->datetime - impl->datetime) < EPSILON;
}



DateTime DateTime::addDays(int days)
{
    DateTime dt(*this);
    dt.impl->datetime += days;
    return dt;
}

DateTime DateTime::addHours(int hours)
{
    DateTime dt(*this);
    dt.impl->datetime += encodeTime(hours, 0, 0);
    return dt;
}

DateTime DateTime::addMinutes(int minutes)
{
    DateTime dt(*this);
    dt.impl->datetime += encodeTime(0, minutes, 0);
    return dt;
}

DateTime DateTime::addSeconds(int seconds)
{
    DateTime dt(*this);
    dt.impl->datetime += encodeTime(0, 0, seconds);
    return dt;
}



DateTime::DateTime(const DateTime& value)
    : impl(new Impl())
{
    this->impl->datetime = value.impl->datetime;
}

DateTime::DateTime()
    : impl(new Impl())
{
}

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second)
    : impl(new Impl())
{
    impl->datetime = encodeDate(year, month, day) + encodeTime(hour, minute, second);
}

DateTime::~DateTime()
{
}

bool DateTime::tryParseDate(std::string str, DateTime& dt, Format::DateFormat format)
{
    const char* s = str.c_str();
    int  yr = 0, mon = 0, day = 0, n;
    char month[4];
    char sep1, sep2;
    dt.impl->datetime = -DateDelta;
    if (strchr(s, '-') || strchr(s, '/'))
    {
        switch (format)
        {
        case Format::Y_M_D:
            n = sscanf(s, "%d%c%d%c%d", &yr, &sep1, &mon, &sep2, &day);
            if ( n < 3 )
            {
                mon = 0;
                n = sscanf(s, "%d%c%3s%c%d", &yr, &sep1, month, &sep2, &day);
                if ( n < 3 )
                    return false;
            }
            break;

        case Format::D_M_Y:
            n = sscanf(s, "%d%c%d%c%d", &day, &sep1, &mon, &sep2, &yr);
            if ( n < 3 )
            {
                mon = 0;
                n = sscanf(s, "%d%c%3s%c%d", &day, &sep1, month, &sep2, &yr);
                if ( n < 3 )
                    return false;
            }
            break;

            default: // M_D_Y
            n = sscanf(s, "%d%c%d%c%d", &mon, &sep1, &day, &sep2, &yr);
            if ( n < 3 )
            {
                mon = 0;
                n = sscanf(s, "%3s%c%d%c%d", month, &sep1, &day, &sep2, &yr);
                if ( n < 3 )
                    return false;
            }
        }

        if (mon == 0)
            mon = findMonth(month);

        dt.impl->datetime = encodeDate(yr, mon, day);
    }

    if (dt.impl->datetime == -DateDelta)
        return false;
    else
        return true;
}

int  findMonth(const char* month)
//  Input:   month = month of year as character string
//  Output:  returns: month of year as a number (1-12)
//  Purpose: finds number (1-12) of month.
{
    int i;
    for (i = 0; i < 12; i++)
    {
        if (UCHAR(month[0]) == MonthTxt[i][0]
        &&  UCHAR(month[1]) == MonthTxt[i][1]
        &&  UCHAR(month[2]) == MonthTxt[i][2]) return i+1;
    }
    return 0;
}
    
void divMod(int n, int d, int* result, int* remainder)
//  Input:   n = numerator
//           d = denominator
//  Output:  result = integer part of n/d
//           remainder = remainder of n/d
//  Purpose: finds integer part and remainder of n/d.
{
    if (d == 0)
    {
        *result = 0;
        *remainder = 0;
    }
    else
    {
        *result = n/d;
        *remainder = n - d*(*result);
    }
}

//=============================================================================

int isLeapYear(int year)
//  Input:   year = a year
//  Output:  returns 1 if year is a leap year, 0 if not
//  Purpose: determines if year is a leap year.
{
    if ((year % 4   == 0)
    && ((year % 100 != 0)
    ||  (year % 400 == 0))) return 1;
    else return 0;
}
    
double encodeDate(int year, int month, int day)
//  Input:   year = a year
//           month = a month (1 to 12)
//           day = a day of month
//  Output:  returns encoded value of year-month-day
//  Purpose: encodes year-month-day to a DateTime value.
{
    int i, j;
    i = isLeapYear(year);
    if ((year >= 1)
        && (year <= 9999)
        && (month >= 1)
        && (month <= 12)
        && (day >= 1)
        && (day <= DaysPerMonth[i][month-1]))
    {
        for (j = 0; j < month-1; j++)
            day += DaysPerMonth[i][j];
        i = year - 1;
        return i*365 + i/4 - i/100 + i/400 + day - DateDelta;
    }
    else
    {
        return -DateDelta;
    }
}

bool DateTime::tryParseTime(std::string str, DateTime& dt)
{
    const char* s = str.c_str();
    int  n, hr, min = 0, sec = 0;

    n = sscanf(s, "%d:%d:%d", &hr, &min, &sec);
    if (n == 0)
        return false;

    double t = encodeTime(hr, min, sec);
    if ((hr >= 0) && (min >= 0) && (sec >= 0))
    {
        dt.impl->datetime = t;
        return true;
    }
    else
    {
        return false;
    }
}

bool DateTime::tryParseTime(std::string str, TimeSpan& dt)
{
    const char* s = str.c_str();
    int  n, hr, min = 0, sec = 0;

    n = sscanf(s, "%d:%d:%d", &hr, &min, &sec);
    if (n == 0)
        return false;

    double t = encodeTime(hr, min, sec);
    if ((hr >= 0) && (min >= 0) && (sec >= 0))
    {
        dt.setTotalSeconds(t * SECS_PER_DAY);
        return true;
    }
    else
    {
        return false;
    }
}

std::string DateTime::toString() const
{
    std::string str;
    char buffer1[20];
    dateToStr(impl->datetime, buffer1, Format::D_M_Y);
    str = buffer1;

    char buffer2[20];
    timeToStr(impl->datetime, buffer2);

    str += " ";
    str += buffer2;

    return str;
}

std::wstring DateTime::toUnicodeString() const
{
    std::string str = toString();
    return std::wstring(str.begin(), str.end());
}

void dateToStr(double date, char* s, Format::DateFormat format)
//  Input:   date = encoded date/time value
//  Output:  s = formatted date string
//  Purpose: represents DateTime date value as a formatted string.
{
    int  y, m, d;
    char dateStr[12];
    decodeDate(date, &y, &m, &d);
    switch (format)
    {
    case Format::Y_M_D:
        sprintf(dateStr, "%4d-%3s-%02d", y, MonthTxt[m-1], d);
        break;

    case Format::M_D_Y:
        sprintf(dateStr, "%3s-%02d-%4d", MonthTxt[m-1], d, y);
        break;

      default:
        sprintf(dateStr, "%02d-%3s-%4d", d, MonthTxt[m-1], y);
    }
    strcpy(s, dateStr);
}

void timeToStr(double time, char* s)
//  Input:   time = decimal fraction of a day
//  Output:  s = time in hr:min:sec format
//  Purpose: represents DateTime time value as a formatted string.
{
    int  hr, min, sec;
    char timeStr[9];
    decodeTime(time, &hr, &min, &sec);
    sprintf(timeStr, "%02d:%02d:%02d", hr, min, sec);
    strcpy(s, timeStr);
}

double encodeTime(int hour, int minute, int second)
//  Input:   hour = hour of day (0-24)
//           minute = minute of hour (0-60)
//           second = seconds of minute (0-60)
//  Output:  returns time encoded as fractional part of a day
//  Purpose: encodes hour:minute:second to a DateTime value
{
    int s;
    if ((hour >= 0) && (minute >= 0) && (second >= 0))
    {
        s = (hour * 3600 + minute * 60 + second);
        return (double)s / SecsPerDay;
    }
    else
    {
        return 0.0;
    }
}


void decodeDate(double date, int* year, int* month, int* day)
//  Input:   date = encoded date/time value
//  Output:  year = 4-digit year
//           month = month of year (1-12)
//           day   = day of month
//  Purpose: decodes DateTime value to year-month-day.
{
    int  D1, D4, D100, D400;
    int  y, m, d, i, k, t;

    D1 = 365;              //365
    D4 = D1 * 4 + 1;       //1461
    D100 = D4 * 25 - 1;    //36524
    D400 = D100 * 4 + 1;   //146097

    t = (int)(floor (date)) + DateDelta;
    if (t <= 0)
    {
        *year = 0;
        *month = 1;
        *day = 1;
    }
    else
    {
        t--;
        y = 1;
        while (t >= D400)
        {
            t -= D400;
            y += 400;
        }
        divMod(t, D100, &i, &d);
        if (i == 4)
        {
            i--;
            d += D100;
        }
        y += i*100;
        divMod(d, D4, &i, &d);
        y += i*4;
        divMod(d, D1, &i, &d);
        if (i == 4)
        {
            i--;
            d += D1;
        }
        y += i;
        k = isLeapYear(y);
        m = 1;
        for (;;)
        {
            i = DaysPerMonth[k][m-1];
            if (d < i) break;
            d -= i;
            m++;
        }
        *year = y;
        *month = m;
        *day = d + 1;
    }
}


void decodeTime(double time, int* h, int* m, int* s)
//  Input:   time = decimal fraction of a day
//  Output:  h = hour of day (0-24)
//           m = minute of hour (0-60)
//           s = second of minute (0-60)
//  Purpose: decodes DateTime value to hour:minute:second.
{
    int secs;
    int mins;
    secs = (int)(floor((time - floor(time))*SecsPerDay + 0.5));
    divMod(secs, 60, &mins, s);
    divMod(mins, 60, h, m);
}
