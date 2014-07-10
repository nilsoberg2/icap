#ifndef __OVERFLOW_H________________________20100409105858__
#define __OVERFLOW_H________________________20100409105858__

#include <map>


class ICAPOverflow
{
public:
    bool IsInEvent(int nodeIdx);
    void Reset(int nodeIdx);
    void Init();

protected:
    std::map<int, bool> eventTracker;
};


#endif//__OVERFLOW_H________________________20100409105858__
