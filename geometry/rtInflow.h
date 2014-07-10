#ifndef RTINFLOW_H__
#define RTINFLOW_H__

#include "inflow.h"
#include "../api.h"


namespace geometry
{
    class ICAP_API RealTimeInflow : public Inflow
    {
    private:
        double inflow;
    public:
        RealTimeInflow() : Inflow(NULL) { }

        void setCurrentInflow(double inflowRate) { this->inflow = inflowRate; }

        virtual double getInflow(double dateTime) { return this->inflow; }
    };
}


#endif//RTINFLOW_H__
