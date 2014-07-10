#ifndef CALCJUNCTION_H__
#define CALCJUNCTION_H__


/*

Library JunctionLib contains a function that calculates the pressure losses in a wye junction using the equations given on pages 401-402 of Hager (1999).  This function uses the IMSL routine BCONF to minimize the sum of squares of errors between 'guessed' values for the upstream and lateral depths and those determined based on the downstream depth and calculated pressure-loss coefficients.  This optimization is bounded by critical depth at the lower end and the downstream depth plus the downstream diameter on the upper end.  If the best solution results in critical depth at one of the inflow branches this is flagged by a negative value for that depth.

The function returns the sum of squares of the error [ real(4 byte)]
The arguments are:
	Ydown	Real (4 byte)	Input	The downstream depth
	QLat	Real (4 byte)	Input	The flow in the lateral branch
	QMain	Real (4 byte)	Input	The flow in the main (upstream) branch
					QMain + QLat is the flow out downstream
	Dd	    Real (4 byte)	Input	The diameter of the downstream branch 
	Dm	    Real (4 byte) 	Input	The diameter of the main (upstream) branch
	Dl	    Real (4 byte) 	Input	The diamter of the lateral branch
	Angle	Real (4 byte)	Input	The angle between the main and lateral branches (degrees)
	Grav	Real (4 byte) 	Input	32.2 ft/sec^2 for Y & D in ft, Q in CFS
					9.81 m/sec^2 for Y and D in m, Q in CMS
	Yup	    Real (4 byte)	Output	The depth in the main (upstream branch)
	YLat	Real (4 byte)	Output	The depth in the lateral branch
	
The following is the visual basic declaration to use this library:

Declare Function CalcJunction Lib "JunctionLib.dll" _
   Alias "_CALCJUNCTION@40" (ByRef YDown As Single, _
   ByRef QLat As Single, ByRef QMain As Single, ByRef Dd As Single, _
   ByRef DMain As Single, ByRef DLat As Single, ByRef Angle As Single, _
   ByRef Grav As Single, ByRef Yup As Single, ByRef YLat As Single) As Single
   
   Public YDown As Single
   Public QLat As Single
   Public QMain As Single
   Public Dd As Single
   Public DMain As Single
   Public DLat As Single
   Public Angle As Single
   Public Grav As Single
   Public Yup As Single
   Public YLat As Single
   Public ErrSq As Single

The following is the actual visual basic call to this function:
    ErrSq = CalcJunction(YDown, QLat, QMain, Dd, DMain, DLat, Angle, Grav, Yup, YLat)
*/


extern "C" float __cdecl CALCJUNCTION(float* YDown, float* QLat, float* QMain, float* Dd,
                                      float* DMain, float* DLat, float* Angle, float* Grav,
                                      float* Yup, float* YLat);


#endif//CALCJUNCTION_H__
