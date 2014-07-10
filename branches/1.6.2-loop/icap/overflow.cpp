
#include "common.h"
#include "overflow.h"


bool ICAPOverflow::IsInEvent(int nodeIdx)
{
    if (eventTracker[nodeIdx])
        return true;
    else
    {
        eventTracker[nodeIdx] = true;
        return false;
    }
}


void ICAPOverflow::Reset(int nodeIdx)
{
    eventTracker[nodeIdx] = false;
}


void ICAPOverflow::Init()
{
    for (int i = 0; i < GNODE_COUNT; i++)
    {
        eventTracker.insert(std::make_pair(i, false));
    }
}
