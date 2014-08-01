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
        impl->errorCode = S_OK;

        // Create a spline for each positive curve.
        for (unsigned int i = 0; i < impl->posValues.size(); i++) {
            //dprintf("Processing %d\n", i);
            hpgvec vec = impl->posValues[i];
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

        return impl->errorCode;
    }

    int Hpg::setupAdvSplines()
    {
        int status = S_OK;
        impl->errorCode = S_OK;

        // Create a spline for each adverse curve.
        for (unsigned int i = 0; i < impl->advValues.size(); i++) {
            //dprintf("Processing -%d\n", i);
            hpgvec vec = impl->advValues[i]; //JJJ
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

        return impl->errorCode;
    }

    int Hpg::setupCritPosSplines()
    {
        if (impl->posCritical.size() == 0)
        {
            // return OK because it's OK to not setup these splines if they're not there
            return S_OK;
        }

        int status = S_OK;
        impl->errorCode = S_OK;

        // Now, create the critical-line splines.  We create three
        // types: the first being f(Q) = upstream, the second being
        // f(Q) = downstream, and the third being f(downstream) = upstream.

        unsigned int numPosValues = (unsigned int)impl->posCritical.size();

        // ds <= us == mild, ds > us == steep
        for (int j = 0; j < numPosValues; j++)
        {
			impl->SplPosCritDS_Q.addPoint(impl->posFlows.at(j), impl->posCritical.at(j).x);
			impl->SplPosCritUS_Q.addPoint(impl->posFlows.at(j), impl->posCritical.at(j).y);
        }

        return impl->errorCode;
    }

    int Hpg::setupCritAdvSplines()

    {
        if (impl->advCritical.size() == 0)
        {
            // return OK because it's OK to not setup these splines if they're not there
            return S_OK;
        }

        int status = S_OK;
        impl->errorCode = S_OK;

        unsigned int numAdvValues = (unsigned int)impl->advCritical.size();

        // ds <= us == mild, ds > us == steep
        for (int j = 0; j < numAdvValues; j++)
        {
			impl->SplAdvCritDS_Q.addPoint(impl->advFlows.at(j), impl->advCritical.at(j).x);
			impl->SplAdvCritUS_Q.addPoint(impl->advFlows.at(j), impl->advCritical.at(j).y);
        }

        return impl->errorCode;
    }

    /**
    * Setup the splines.  setupSplines sets up splines for every HPG
    * curve and for the critical depth curve.  It should be run after
    * loading/creation has taken place.
    */
    int Hpg::setupSplines()
    {
        int status = S_OK;
        impl->errorCode = S_OK;

        // We can't create splines if there aren't any curves!
        if (! impl->posValues.size() && ! impl->advValues.size())
        {
            impl->errorCode = err::InvalidSplineSize;
            return impl->errorCode;
        }

        if (impl->posValues.size() > 0)
        {
            if (setupPosSplines() != S_OK)
                return impl->errorCode;
            //if (setupCritPosSplines() != S_OK)
            //    return errorCode;
        }

        if (impl->advValues.size() > 0)
        {
            if (setupAdvSplines() != S_OK)
                return impl->errorCode;
            //if (setupCritAdvSplines() != S_OK)
            //    return errorCode;
        }

        return S_OK;
    }

}
