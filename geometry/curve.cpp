
#include <algorithm>
#include <sstream>
#include "curve.h"
#include "parse.h"


namespace geometry
{
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

    const std::string& Curve::getName() const
    {
        return this->name;
    }

    const std::string& Curve::getType() const
    {
        return this->type;
    }

    void Curve::addEntry(double x, double y)
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

        double val = this->xVals[0];
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

    double interpolate(double xVal, double x1, double x2, double y1, double y2)
    {
        double dx = x2 - x1;
        if (fabs(dx) < 1.0e-20)
        {
            return (y1 + y2) / 2.;
        }
        else
        {
            return y1 + (xVal - x1) * (y2 - y1) / dx;
        }
    }

    double Curve::lookup(double x) const
    {
        if (this->xVals.size() < 2)
        {
            return 0;
        }

        if (x <= this->xVals.front())
        {
            return this->yVals.front();
        }
        else if (x >= this->xVals.back())
        {
            return this->yVals.back();
        }
        else
        {
            for (int i = 1; i < this->xVals.size(); i++)
            {
                if (this->xVals[i] > x)
                {
                    return interpolate(x, this->xVals[i - 1], this->xVals[i], this->yVals[i - 1], this->yVals[i]);
                }
            }
            return 0;
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

