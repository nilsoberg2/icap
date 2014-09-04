
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "../util/parse.h"
#include "../time/datetime.h"

#include "timeseries.h"


namespace geometry
{
    Timeseries::Timeseries(std::string name)
        : Curve(name, "timeseries")
    {
    }

    void Timeseries::setStartDateTime(const DateTime& dateTime)
    {
        this->lastDate = dateTime;
    }

    bool Timeseries::parseLine(const std::vector<std::string>& parts)
    {
        using namespace std;
        using namespace boost::algorithm;

        if (parts.size() < 3)
        {
            setErrorMessage("At least 3 parts required for TIMESERIES line");
            return false;
        }

        if (parts[1] == "file")
        {
            string path = parts[2];
            replace_all(path, "\"", "");
            replace_all(path, "\'", "");
            if (!loadFromFile(path))
            {
                return false;
            }
        }
        else
        {
            if (!parseLine(parts, true))
            {
                return false;
            }
        }

        return true;
    }

    enum LineState
    {
        FindDate,
        FindTime,
        FindValue,
    };

    bool Timeseries::parseLine(const std::vector<std::string>& parts, bool hasName)
    {
        int startIndex = 0;
        if (hasName)
        {
            startIndex = 1;
        }
        
        DateTime time;
        int i = startIndex;
        LineState state = LineState::FindDate;
        while (i < parts.size())
        {
            if (state == LineState::FindDate)
            {
                DateTime date;
                if (DateTime::tryParseDate(parts[i].c_str(), date))
                {
                    this->lastDate = date;
                    i++;
                }
                state = LineState::FindTime;
            }

            else if (state == LineState::FindTime)
            {
                double temp;
                // First check for decimal hours time format.
                if (tryParse(parts[i], temp))
                {
                    temp /= 24.0;
                    time = temp + this->lastDate;
                }
                // If it's not a decimal hours format, then assume it's a HH:MM[:SS] format.
                else if (DateTime::tryParseTime(parts[i].c_str(), time))
                {
                    time += this->lastDate;
                }
                else
                {
                    setErrorMessage("Unable to convert '" + parts[i] + "' to a Date/Time");
                    return false;
                }

                state = LineState::FindValue;
                i++;
            }

            else if (state == LineState::FindValue)
            {
                // Try to parse a numeric value.
                double val;
                if (!tryParse(parts[i], val))
                {
                    setErrorMessage("Unable to convert '" + parts[i] + "' to a value");
                    return false;
                }

                this->addEntry(time, val);

                i++;
                state = LineState::FindDate;
            }
        }

        return true;
    }

    bool Timeseries::loadFromFile(const std::string& filePath)
    {
        using namespace std;
        using namespace boost::algorithm;

        ifstream input(filePath);

        if (!input.good())
        {
            setErrorMessage("Unable to load timeseries file '" + filePath + "'");
            return false;
        }
        
        string line;
        while (getline(input, line).good())
        {
            vector<string> parts;
            split(parts, line, is_any_of(" \t"), token_compress_on);
            if (!parseLine(parts, false))
            {
                return false;
            }
        }

        return true;
    }
}
