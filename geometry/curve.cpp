
#include <algorithm>
#include <sstream>

#include "../util/parse.h"

#include "curve.h"


namespace geometry
{
    var_type lookupEx(double x, const std::vector<var_type>& xVals, const std::vector<var_type>& yVals);


    Curve::Curve(std::string theName)
    {
        Init(theName, "");
    }

    Curve::Curve(std::string theName, std::string theType)
    {
        Init(theName, theType);
    }

    void Curve::Init(std::string theName, std::string theType)
    {
        this->name = theName;
        this->type = theType;
    }
    
    void Curve::getFirstPoint(var_type& x, var_type& y)
    {
        if (this->xVals.size() > 0)
        {
            x = *this->xVals.begin();
            y = *this->yVals.begin();
        }
    }

    void Curve::getLastPoint(var_type& x, var_type& y)
    {
        if (this->xVals.size() > 0)
        {
            x = *this->xVals.end();
            y = *this->yVals.end();
        }
    }

    const std::string& Curve::getName() const
    {
        return this->name;
    }

    const std::string& Curve::getType() const
    {
        return this->type;
    }

    void Curve::addEntry(var_type x, var_type y)
    {
        this->xVals.push_back(x);
        this->yVals.push_back(y);
    }

    bool Curve::validate()
    {
        if (this->xVals.size() < 2)
        {
            return false;
        }

        var_type val = this->xVals[0];
        for (int i = 1; i < this->xVals.size(); i++)
        {
            if (this->xVals[i] <= val)
            {
                return false;
            }
            val = this->xVals[i];
        }

        return true;
    }

    var_type interpolate(var_type xVal, var_type x1, var_type x2, var_type y1, var_type y2)
    {
        var_type dx = x2 - x1;
        if (fabs(dx) < 1.0e-20)
        {
            return (y1 + y2) / 2.;
        }
        else
        {
            return y1 + (xVal - x1) * (y2 - y1) / dx;
        }
    }

    var_type Curve::inverseLookup(var_type y) const
    {
        return lookupEx(y, this->yVals, this->xVals);
    }

    var_type Curve::lookup(var_type x) const
    {
        return lookupEx(x, this->xVals, this->yVals);
    }

    var_type lookupEx(double x, const std::vector<var_type>& xVals, const std::vector<var_type>& yVals)
    {
        if (xVals.size() < 2)
        {
            return var_type();
        }

        if (x <= xVals.front())
        {
            return yVals.front();
        }
        else if (x >= xVals.back())
        {
            return yVals.back();
        }
        else
        {
            for (int i = 1; i < xVals.size(); i++)
            {
                if (xVals[i] > x)
                {
                    return interpolate(x, xVals[i - 1], xVals[i], yVals[i - 1], yVals[i]);
                }
            }
            return var_type();
        }
    }

    bool Curve::parseLine(const std::vector<std::string>& parts)
    {
        if (parts.size() < 3)
        {
            return false;
        }

        int startIdx = 1;
        if (parts.size() >= 4)
        {
            startIdx = 2;
            this->type = parts[1];
        }

        for (int i = startIdx; i < parts.size(); i += 2)
        {
            double x;
            if (!tryParse(parts[i], x))
            {
                setErrorMessage("Unable to parse curve X");
                return false;
            }
            double y;
            if (!tryParse(parts[i+1], y))
            {
                setErrorMessage("Unable to parse curve Y");
                return false;
            }

            addEntry(x, y);
        }

        return true;
    }
}

