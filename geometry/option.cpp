
#include <vector>
#include <boost/algorithm/string.hpp>
#include "option.h"

namespace geometry {

    bool Option::parseLine(const std::string& line)
    {
        using namespace std;
        using namespace boost::algorithm;

        vector<string> parts;
        split(parts, line, is_any_of(" \t"), token_compress_on);

        if (parts.size() == 0)
        {
            return true;
        }

        this->name = parts[0];

        if (parts.size() == 1)
        {
            return true;
        }
        else
        {
            string val = line.substr(this->name.length());
            trim(val);
            if (val.length() >= 2 && (val[0] == '"' || val[0] == '\''))
            {
                this->value = val.substr(1, val.length() - 2);
            }
            else
            {
                this->value = val;
            }
        }

        return true;
    }

}
