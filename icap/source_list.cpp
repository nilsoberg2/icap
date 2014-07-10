

#include "source_list.h"
#define NULL 0


Source SourceList::GetSource(int idx)
{
    if (idx >= 0 && idx < (int)sources.size())
        return sources[idx];
    else
    {
        Source temp;
        temp.active = false;
        return temp;
    }
}


int SourceList::AddSource(Source src)
{
    sources.push_back(src);
    return (int)sources.size() - 1;
}


int SourceList::Count()
{
    return (int)sources.size();
}
