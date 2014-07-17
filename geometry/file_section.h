#ifndef FILESECTION_H__
#define FILESECTION_H__

#include <map>
#include <string>

namespace geometry
{

    enum FileSection
    {
        File_Option,
        File_Junction,
        File_Storage,
        File_Conduit,
        File_Xsection,

        File_Inflow,
        File_Curve,
        File_Timeseries,

        File_Coordinate,
        File_Vertex,

        File_None,
    };

    void populateSectionNameMap(std::map<std::string, FileSection>& map);

}


#endif//FILESECTION_H__
