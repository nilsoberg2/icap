#ifndef OPTIONS_H__
#define OPTIONS_H__

#include <memory>
#include <vector>
#include <string>


namespace geometry
{
    class Options
    {
    public:
        virtual std::vector<std::string> getOptionNames() const = 0;
        virtual std::string getOption(std::string optionName) const = 0;
        virtual bool hasOption(std::string optionName) const = 0;
        virtual Options* asOptions() = 0;
    };
}


#endif//OPTIONS_H__
