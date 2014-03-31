#ifndef __SOURCE_LIST_H_____________________20080423095959__
#define __SOURCE_LIST_H_____________________20080423095959__

#include <vector>

struct Source
{
    int link;
    int node;
    bool active;
};

class SourceList
{
protected:
    std::vector<Source> sources;
public:
    Source GetSource(int idx);
    int AddSource(Source src);
    int Count();
};


#endif//__SOURCE_LIST_H_____________________20080423095959__
