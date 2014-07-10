#ifndef DATETIME_H__
#define DATETIME_H__

#include <memory>
#include <string>


namespace Format
{
    enum DateFormat
    {
        Y_M_D,
        M_D_Y,
        D_M_Y,
    };
}

class DateTime
{
private:
    class Impl;
    std::unique_ptr<Impl> impl;
    //Impl* impl;

public:
    DateTime();
    DateTime(int year, int month, int day, int hour, int minute, int second);
    ~DateTime();
    DateTime(const DateTime& value);

    operator double();
    DateTime& operator /=(const double& value);
    DateTime& operator +=(const double& value);
    DateTime& operator +=(const DateTime& value);
    DateTime operator +(const DateTime& value) const;
    DateTime& operator =(const DateTime& value);
    DateTime& operator =(const double& value);
    bool operator ==(const DateTime& value) const;
    
    static bool tryParseDate(const char* str, DateTime& theDateTime, Format::DateFormat format = Format::M_D_Y);
    static bool tryParseTime(const char* str, DateTime& theDateTime);

    std::string toString() const;
    std::wstring toUnicodeString() const;

    DateTime addDays(int days);
    DateTime addHours(int hours);
    DateTime addMinutes(int minutes);
    DateTime addSeconds(int seconds);
};


#endif//DATETIME_H__

