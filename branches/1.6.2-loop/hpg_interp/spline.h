#ifndef SPLINE_WRAPPER_H_
#define SPLINE_WRAPPER_H_

#include <vector>

#include "spline-tk.h"


typedef tk::spline Spline;

#define SPL_INIT_TEMP(Sname, isLinear)          tk::spline Sname; std::vector<double> X_##Sname##, Y_##Sname##; bool isLin_##Sname## = isLinear;
#define SPL_INIT_NOCREATE(Sname, isLinear)      std::vector<double> X_##Sname##, Y_##Sname##; bool isLin_##Sname## = isLinear;
//#define SPL_INIT_TEMP(Sname, isLinear)          magnet::math::Spline ##Sname##; if (isLinear) { ##Sname##.setType(magnet::math::Spline::Spline_type::LINEAR); }
#define SPL_ADD_TEMP(Sname, x, y)               X_##Sname##.push_back(x); Y_##Sname##.push_back(y);
#define SPL_ADD_NOCREATE(Sname, x, y)           X_##Sname##.push_back(x); Y_##Sname##.push_back(y);
#define SPL_FINISH_TEMP(Sname)                  Sname.set_points(X_##Sname##, Y_##Sname##, isLin_##Sname##);
#define SPL_FINISH_NOCREATE(Sname, Varname)     Varname.set_points(X_##Sname##, Y_##Sname##, isLin_##Sname##);


#endif//SPLINE_WRAPPER_H_
