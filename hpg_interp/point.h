#ifndef POINT_H__
#define POINT_H__



// Begin wrapping code in the HPG namespace
namespace hpg
{


    // This structure defines an elevation tuple.  One tuple holds an X value,
    // a Y value, and a Volume value.

    struct point
    {
        double  x;
        bool    x_valid;
        double  y;
        bool    y_valid;
        double  v;
        bool    v_valid;
		double  hf;
		bool    hf_valid;
        point(const double& x_, const double& y_, const double& v_, const double& hf_)
			{x=x_; y=y_; v=v_; hf=hf_; x_valid = y_valid = v_valid = hf_valid = true;}
        point(const double& x_, const double& y_, const double& v_)
			{x=x_; y=y_; v=v_; x_valid = y_valid = v_valid = true; hf_valid = false;}
        point(const double& x_, const double& y_)
			{x=x_; y=y_; x_valid = y_valid = true; v_valid = hf_valid = false;}
        point()
			{x=0.0; y=0.0; v=0.0; hf = 0.0; x_valid = y_valid = v_valid = hf_valid = false;}
        point& operator=(const point& in)
			{x=in.x; y=in.y; v=in.v; hf=in.hf; x_valid=in.x_valid; y_valid=in.y_valid; v_valid=in.v_valid; hf_valid=in.hf_valid; return *this;}
    };

}


#endif//POINT_H__
