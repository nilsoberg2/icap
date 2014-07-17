#ifndef TIMESERIES_H__
#define TIMESERIES_H__

#include <string>
#include <vector>

#include "../util/parseable.h"
#include "../time/datetime.h"
#include "../api.h"

#include "curve.h"


namespace geometry
{
    class Timeseries : public Curve
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
