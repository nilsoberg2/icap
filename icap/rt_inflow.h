#ifndef RT_INFLOW_H__
#define RT_INFLOW_H__

#include "../geometry/inflow.h"


class RealTimeInflow : public geometry::Inflow
{
private:
    double inflow;
public:
    RealTimeInflow() : Inflow(NULL) { }

    void setCurrentInflow(double inflowRate) { this->inflow = inflowRate; }

    virtual double getInflow(double dateTime) { return this->inflow; }
};


#endif//RT_INFLOW_H__
