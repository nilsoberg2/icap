
#include "units.h"


double gravity[2] = {9.81, 32.174};


Units::Units(UnitSystem sys)
{
    this->system = sys;
}


double Units::g()
{
    return gravity[this->system];
}
