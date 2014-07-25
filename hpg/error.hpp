#ifndef __ERROR_HPP_________________________20050127162121__
#define __ERROR_HPP_________________________20050127162121__


namespace hpg
{
	namespace error
	{
		const int bad_value = -99993;
		const int divergence = -99995;
		const int invalid_discharge = -99996;
		const int invalid_format = -100000;
		const int open_failed = -99998;
		const int empty_file = -99999;
		const int uninterpolable = -99994;
		const int invalid_curve_id = -99991;
		const int no_points = -99992;
		const int at_max_depth = -99990;
		const int imaginary = -99987;
        const int at_min_depth = -99986;
	};
}


#if !defined(S_OK)
#define S_OK              0
#endif
#define HPGFAILURE(XXX)   XXX != S_OK


#endif//__ERROR_HPP_________________________20050127162121__
