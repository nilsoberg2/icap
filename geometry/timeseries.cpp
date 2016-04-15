// ==============================================================================
// ICAP License
// ==============================================================================
// University of Illinois/NCSA
// Open Source License
// 
// Copyright (c) 2014-2016 University of Illinois at Urbana-Champaign.
// All rights reserved.
// 
// Developed by:
// 
//     Nils Oberg
//     Blake J. Landry, PhD
//     Arthur R. Schmidt, PhD
//     Ven Te Chow Hydrosystems Lab
// 
//     University of Illinois at Urbana-Champaign
// 
//     https://vtchl.illinois.edu
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal with
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimers.
// 
//     * Redistributions in binary form must reproduce the above copyright notice,
//       this list of conditions and the following disclaimers in the
//       documentation and/or other materials provided with the distribution.
// 
//     * Neither the names of the Ven Te Chow Hydrosystems Lab, University of
// 	  Illinois at Urbana-Champaign, nor the names of its contributors may be
// 	  used to endorse or promote products derived from this Software without
// 	  specific prior written permission.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
// SOFTWARE.


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
