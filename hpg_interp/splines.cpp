#include "hpg.hpp"
#include "errors.hpp"
#include "debug.h"
#include "spline2.hpp"
#include "impl.h"

namespace hpg
{

    int Hpg::setupPosSplines()
    {
        int status = S_OK;
        impl->ErrorCode = S_OK;

        // Create a spline for each positive curve.
        for (unsigned int i = 0; i < impl->PosValues.size(); i++) {
            //dprintf("Processing %d\n", i);
            hpgvec vec = impl->PosValues[i];
            unsigned int numValues = (unsigned int)vec.size();

            // Check if there are more than the minimum required number
            // of points for that spline.
            if (numValues >= 2) {
				magnet::math::Spline spline;
                //std::vector<double> usY, usX;
                magnet::math::Spline usSpline;
                usSpline.setType(magnet::math::Spline::Spline_type::LINEAR);
				magnet::math::Spline volSpline;
				magnet::math::Spline hfSpline;
                double lastY = 0;
				for (unsigned int j = 0; j < numValues; j++)
				{
					spline.addPoint(vec[j].x, vec[j].y);
                    if (j == 0 || (j > 0 && fabs(lastY - vec[j].y) > 0.0001))
                    {
                        //usY.push_back(vec[j].y);
                        //usX.push_back(vec[j].x);
                        usSpline.addPoint(vec[j].y, vec[j].x);
                    }
                    lastY = vec[j].y;
					volSpline.addPoint(vec[j].x, vec[j].v);
					hfSpline.addPoint(vec[j].x, vec[j].hf);
				}

                //spline.generate();
                //usSpline.generate();
                //volSpline.generate();

                impl->SplPosUS_QDS.push_back(spline);
                //SplPosDS_QUS.push_back(devinlane::Spline<double, double>(usY, usX));
                impl->SplPosDS_QUS.push_back(usSpline);
				impl->SplPosVol.push_back(volSpline);
				impl->SplPosHf.push_back(hfSpline);
            }

            // If there aren't enough points, then create a null spline.
            else
            {
                impl->SplPosUS_QDS.push_back(magnet::math::Spline());
                //SplPosDS_QUS.push_back(devinlane::Spline<double, double>());
                impl->SplPosDS_QUS.push_back(magnet::math::Spline());
				impl->SplPosVol.push_back(magnet::math::Spline());
				impl->SplPosHf.push_back(magnet::math::Spline());
            }
        }

        return impl->ErrorCode;
    }

    int Hpg::setupAdvSplines()
    {
        int status = S_OK;
        impl->ErrorCode = S_OK;

        // Create a spline for each adverse curve.
        for (unsigned int i = 0; i < impl->AdvValues.size(); i++) {
            //dprintf("Processing -%d\n", i);
            hpgvec vec = impl->AdvValues[i]; //JJJ
            unsigned int numValues = (unsigned int)vec.size();

            // Check if there are more than the minimum required number
            // of points for that spline.
            if (numValues >= 2) {
				magnet::math::Spline spline;
                magnet::math::Spline usSpline;
                usSpline.setType(magnet::math::Spline::Spline_type::LINEAR);
				magnet::math::Spline volSpline;
				magnet::math::Spline hfSpline;
                double lastY = 0;
				for (unsigned int j = 0; j < numValues; j++)
				{
					spline.addPoint(vec[j].x, vec[j].y);
                    if (j == 0 || (j > 0 && fabs(lastY - vec[j].y) > 0.0001))
                    {
                        usSpline.addPoint(vec[j].y, vec[j].x);
                    }
                    lastY = vec[j].y;
					volSpline.addPoint(vec[j].x, vec[j].v);
					hfSpline.addPoint(vec[j].x, vec[j].hf);
				}

                
                //spline.generate();
                //usSpline.generate();
                //volSpline.generate();

                impl->SplAdvUS_QDS.push_back(spline);
                impl->SplAdvDS_QUS.push_back(usSpline);
				impl->SplAdvVol.push_back(volSpline);
				impl->SplAdvHf.push_back(hfSpline);
            }

            // If there aren't enough points, then create a null spline.
            else
            {
                impl->SplAdvUS_QDS.push_back(magnet::math::Spline());
                impl->SplAdvDS_QUS.push_back(magnet::math::Spline());
				impl->SplAdvVol.push_back(magnet::math::Spline());
				impl->SplAdvHf.push_back(magnet::math::Spline());
            }
        }

        return impl->ErrorCode;
    }

    int Hpg::setupCritPosSplines()
    {
        if (impl->PosCritical.size() == 0)
        {
            // return OK because it's OK to not setup these splines if they're not there
            return S_OK;
        }

        int status = S_OK;
        impl->ErrorCode = S_OK;

        // Now, create the critical-line splines.  We create three
        // types: the first being f(Q) = upstream, the second being
        // f(Q) = downstream, and the third being f(downstream) = upstream.

        unsigned int numPosValues = (unsigned int)impl->PosCritical.size();

        // ds <= us == mild, ds > us == steep
        for (int j = 0; j < numPosValues; j++)
        {
			impl->SplPosCritDS_Q.addPoint(impl->PosFlows.at(j), impl->PosCritical.at(j).x);
			impl->SplPosCritUS_Q.addPoint(impl->PosFlows.at(j), impl->PosCritical.at(j).y);
        }

        return impl->ErrorCode;
    }

    int Hpg::setupCritAdvSplines()

    {
        if (impl->AdvCritical.size() == 0)
        {
            // return OK because it's OK to not setup these splines if they're not there
            return S_OK;
        }

        int status = S_OK;
        impl->ErrorCode = S_OK;

        unsigned int numAdvValues = (unsigned int)impl->AdvCritical.size();

        // ds <= us == mild, ds > us == steep
        for (int j = 0; j < numAdvValues; j++)
        {
			impl->SplAdvCritDS_Q.addPoint(impl->AdvFlows.at(j), impl->AdvCritical.at(j).x);
			impl->SplAdvCritUS_Q.addPoint(impl->AdvFlows.at(j), impl->AdvCritical.at(j).y);
        }

        return impl->ErrorCode;
    }

    /**
    * Setup the splines.  setupSplines sets up splines for every HPG
    * curve and for the critical depth curve.  It should be run after
    * loading/creation has taken place.
    */
    int Hpg::SetupSplines()
    {
        int status = S_OK;
        impl->ErrorCode = S_OK;

        // We can't create splines if there aren't any curves!
        if (! impl->PosValues.size() && ! impl->AdvValues.size())
        {
            impl->ErrorCode = err::InvalidSplineSize;
            return impl->ErrorCode;
        }

        if (impl->PosValues.size() > 0)
        {
            if (setupPosSplines() != S_OK)
                return impl->ErrorCode;
            //if (setupCritPosSplines() != S_OK)
            //    return ErrorCode;
        }

        if (impl->AdvValues.size() > 0)
        {
            if (setupAdvSplines() != S_OK)
                return impl->ErrorCode;
            //if (setupCritAdvSplines() != S_OK)
            //    return ErrorCode;
        }

        return S_OK;
    }

}
