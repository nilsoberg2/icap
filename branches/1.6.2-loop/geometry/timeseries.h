#ifndef TIMESERIES_H__
#define TIMESERIES_H__

#include <string>
#include <vector>
#include "curve.h"
#include "parseable.h"
#include "../time/datetime.h"
#include "../api.h"


namespace geometry
{
    class ICAP_API Timeseries : public Curve
    {
    private:
        DateTime lastDate;

        bool loadFromFile(const std::string& filePath);
        // Can handle multiple date/time-value pairs on a single line
        bool parseLine(const std::vector<std::string>& parts, bool hasName);

    public:
        Timeseries(std::string name);
        bool parseLine(const std::vector<std::string>& parts);
        void setStartDateTime(DateTime dateTime);
    };
}


#endif//TIMESERIES_H__
