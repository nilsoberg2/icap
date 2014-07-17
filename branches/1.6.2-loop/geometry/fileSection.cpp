
#include <map>

#include "file_section.h"


namespace geometry
{
    void populateSectionNameMap(std::map<std::string, FileSection>& map)
    {
        map.insert(std::pair<std::string, FileSection>("options", FileSection::File_Option));
        map.insert(std::pair<std::string, FileSection>("extended_options", FileSection::File_Option));
        map.insert(std::pair<std::string, FileSection>("junctions", FileSection::File_Junction));
        map.insert(std::pair<std::string, FileSection>("storage", FileSection::File_Storage));
        map.insert(std::pair<std::string, FileSection>("conduits", FileSection::File_Conduit));
        map.insert(std::pair<std::string, FileSection>("xsections", FileSection::File_Xsection));
        map.insert(std::pair<std::string, FileSection>("curves", FileSection::File_Curve));
        map.insert(std::pair<std::string, FileSection>("timeseries", FileSection::File_Timeseries));
        map.insert(std::pair<std::string, FileSection>("inflows", FileSection::File_Inflow));
        map.insert(std::pair<std::string, FileSection>("coordinates", FileSection::File_Coordinate));
        map.insert(std::pair<std::string, FileSection>("vertices", FileSection::File_Vertex));
    }
}