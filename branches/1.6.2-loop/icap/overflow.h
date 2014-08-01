#ifndef __OVERFLOW_H________________________20100409105858__
#define __OVERFLOW_H________________________20100409105858__

#include <map>

#include "../geometry/node_list.h"


class IcapOverflow
{
public:
    bool IsInEvent(int nodeIdx);
    void Reset(int nodeIdx);
    void Init(geometry::NodeList* nodeList);

protected:
    std::map<int, bool> eventTracker;
};


#endif//__OVERFLOW_H________________________20100409105858__
