#include "hpg.hpp"
#include "errors.hpp"
#include "debug.h"
#include "spline.h"
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
				SPL_INIT_TEMP(spline, false);
                SPL_INIT_TEMP(usSpline, false);
				SPL_INIT_TEMP(volSpline, false);
				SPL_INIT_TEMP(hfSpline, false);
                double lastY = 0;
				for (unsigned int j = 0; j < numValues; j++)
				{
					SPL_ADD_TEMP(spline, vec[j].x, vec[j].y);
                    if (j == 0 || (j > 0 && (vec[j].y - lastY) > 0.0001))
                    {
                        lastY = vec[j].y;
                        SPL_ADD_TEMP(usSpline, vec[j].y, vec[j].x);
                    }
					SPL_ADD_TEMP(volSpline, vec[j].x, vec[j].v);
					SPL_ADD_TEMP(hfSpline, vec[j].x, vec[j].hf);
				}

                SPL_FINISH_TEMP(spline);
                SPL_FINISH_TEMP(usSpline);
                SPL_FINISH_TEMP(volSpline);
                SPL_FINISH_TEMP(hfSpline);

                impl->SplPosUS_QDS.push_back(spline);
                //// When creating this spline, the values that are not in ascending order are removed.
                impl->SplPosDS_QUS.push_back(usSpline);
                //impl->SplPosDS_QUS.push_back(Spline());
				impl->SplPosVol.push_back(volSpline);
				impl->SplPosHf.push_back(hfSpline);
            }

            // If there aren't enough points, then create a null spline.
            else
            {
                impl->SplPosUS_QDS.push_back(Spline());
                impl->SplPosDS_QUS.push_back(Spline());
				impl->SplPosVol.push_back(Spline());
				impl->SplPosHf.push_back(Spline());
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
				SPL_INIT_TEMP(spline, false);
                SPL_INIT_TEMP(usSpline, false);
				SPL_INIT_TEMP(volSpline, false);
				SPL_INIT_TEMP(hfSpline, false);
                double lastY = 0;
				for (unsigned int j = 0; j < numValues; j++)
				{
					SPL_ADD_TEMP(spline, vec[j].x, vec[j].y);
                    if (j == 0 || (j > 0 && fabs(lastY - vec[j].y) > 0.0001))
                    {
                        SPL_ADD_TEMP(usSpline, vec[j].y, vec[j].x);
                    }
                    lastY = vec[j].y;
					SPL_ADD_TEMP(volSpline, vec[j].x, vec[j].v);
					SPL_ADD_TEMP(hfSpline, vec[j].x, vec[j].hf);
				}

                SPL_FINISH_TEMP(spline);
                SPL_FINISH_TEMP(usSpline);
                SPL_FINISH_TEMP(volSpline);
                SPL_FINISH_TEMP(hfSpline);

                impl->SplAdvUS_QDS.push_back(spline);
                impl->SplAdvDS_QUS.push_back(usSpline);
				impl->SplAdvVol.push_back(volSpline);
				impl->SplAdvHf.push_back(hfSpline);
            }

            // If there aren't enough points, then create a null spline.
            else
            {
                impl->SplAdvUS_QDS.push_back(Spline());
                impl->SplAdvDS_QUS.push_back(Spline());
				impl->SplAdvVol.push_back(Spline());
				impl->SplAdvHf.push_back(Spline());
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

        SPL_INIT_NOCREATE(SplPosCritDS_Q, false);
        SPL_INIT_NOCREATE(SplPosCritUS_Q, false);

        // ds <= us == mild, ds > us == steep
        for (int j = 0; j < numPosValues; j++)
        {
			SPL_ADD_NOCREATE(SplPosCritDS_Q, impl->posFlows.at(j), impl->posCritical.at(j).x);
			SPL_ADD_NOCREATE(SplPosCritUS_Q, impl->posFlows.at(j), impl->posCritical.at(j).y);
        }

        SPL_FINISH_NOCREATE(SplPosCritDS_Q, impl->SplPosCritDS_Q);
        SPL_FINISH_NOCREATE(SplPosCritUS_Q, impl->SplPosCritUS_Q);

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
        
        SPL_INIT_NOCREATE(SplAdvCritDS_Q, false);
        SPL_INIT_NOCREATE(SplAdvCritUS_Q, false);

        // ds <= us == mild, ds > us == steep
        for (int j = 0; j < numAdvValues; j++)
        {
			SPL_ADD_NOCREATE(SplAdvCritDS_Q, impl->advFlows.at(j), impl->advCritical.at(j).x);
			SPL_ADD_NOCREATE(SplAdvCritUS_Q, impl->advFlows.at(j), impl->advCritical.at(j).y);
        }

        SPL_FINISH_NOCREATE(SplAdvCritDS_Q, impl->SplAdvCritDS_Q);
        SPL_FINISH_NOCREATE(SplAdvCritUS_Q, impl->SplAdvCritUS_Q);

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
