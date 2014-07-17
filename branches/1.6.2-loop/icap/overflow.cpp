

#include "overflow.h"


bool IcapOverflow::IsInEvent(int nodeIdx)
{
    if (eventTracker[nodeIdx])
        return true;
    else
    {
        eventTracker[nodeIdx] = true;
        return false;
    }
}


void IcapOverflow::Reset(int nodeIdx)
{
    eventTracker[nodeIdx] = false;
}


void IcapOverflow::Init(std::shared_ptr<geometry::NodeList> nodeList)
{
    for (int i = 0; i < nodeList->count(); i++)
    {
        eventTracker.insert(std::make_pair(i, false));
    }
}
