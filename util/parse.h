#ifndef PARSE_H__
#define PARSE_H__


#include <string>
#include <sstream>


template<typename T>
bool tryParse(const std::string& str, T& value)
{
    std::stringstream stream(str);
    char test;

    // First test is to check if the value is indeed numeric; the second test is to check if something else
    // is in the stream which would mean that the string is not numeric.
    if (!(stream >> value) || stream >> test)
    {
        return false;
    }
    else
    {
        return true;
    }
}


#endif//PARSE_H__
