#pragma warning(disable : 4786) //disable warnings on identifier truncated to 255 chars
#include <math.h>

#include "errors.hpp"
#include "hpg.hpp"
#include "impl.h"

using namespace std;

namespace hpg
{

    /// Get the upstream value given the downstream value and the flow.
    /// This will select the proper interpolation/extrapolation routine
    /// and perform the interpolation/extrapolation.
    int Hpg::InterpUpstreamHead(double flow, double downstream, double& result)
    {
        impl->errorCode = S_OK;

        // Get the Q_lower flow index
        unsigned int curve;
        int status = S_OK;
        if (HPGFAILURE(status = findLowerBracketingCurve(flow, curve)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // Get the first point on the Q_lower curve
        point c1firstp;
        if (HPGFAILURE(status = getFirstPointOnCurve(flow, curve, c1firstp)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // Get the first point on the Q_upper curve
        point c2firstp;
        if (HPGFAILURE(status = getFirstPointOnCurve(flow, curve + 1, c2firstp)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // Get the last point on the Q_lower curve. This is used to determine
        // if we need to perform extrapolation or interpolation.
        point lastp;
        if (HPGFAILURE(status = getLastPointOnCurve(flow, curve, lastp)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // steepness from isCurveSteep: negative = steep, positive = mild, 0 = error
        bool steepC1 = false;
        bool steepC2 = false;

        // Make sure that the flow is positive before computing the steepness.
        if (flow >= 0.0)
        {
            steepC1 = isCurveSteep(curve);
            steepC2 = isCurveSteep(curve + 1);
        }

        // This tests to see if Q_lower is steep.
        if (steepC1)
        {
            // If Q_lower and Q_upper are steep, then we do steep interpolation.
            if (steepC2)
            {
                double upstream;

                // If our downstream point is less than the downstream critical
                // value for our flow, then we use the upstream critical value
                // for the result.
                if (downstream < c1firstp.x)
                {
                    double flow1, flow2;
                    if (flow >= 0.0)
                    {
                        flow1 = impl->posFlows.at(curve);
                        flow2 = impl->posFlows.at(curve + 1);
                    }
                    else
                    {
                        flow1 = impl->advFlows.at(curve);
                        flow2 = impl->advFlows.at(curve + 1);
                    }
                    upstream = linearInterp(flow, flow1, c1firstp.y, flow2, c2firstp.y);
                }

                // Else, if our downstream is less than the first point (critical)
                // on the Q_upper curve, then we do a special type of interpolation.
                else if (downstream < c2firstp.x)
                {
                    if (HPGFAILURE(status = interpolateSteepSpecial(curve, flow, downstream, upstream)))
                    {
                        impl->errorCode = status;
                        return impl->errorCode;
                    }
                }

                // Else, if we are more than the last downstream point on the curve,
                // then we extrapolate.
                else if (downstream > lastp.x)
                {
                    if (HPGFAILURE(status = standardExtrapolation(curve, flow, downstream, upstream)))
                    {
                        impl->errorCode = status;
                        return impl->errorCode;
                    }
                }

                // Otherwise, we do standard interpolation.
                else
                {
                    if (HPGFAILURE(status = standardInterpolation(curve, flow, downstream, upstream, InterpValue::Interp_Upstream)))
                    {
                        impl->errorCode = status;
                        return impl->errorCode;
                    }
                }

                // Save the result.
                result = upstream;

            }//end if (steepC2)

            // Else, if Q_lower is steep and Q_upper is mild, then we are
            // in a transitional region.
            else
            {
                double upstream = (c1firstp.y + c2firstp.y) / 2.;
                result = upstream;
            }//end else
        }//end if (steepC1)

        // If the matching if conditional is false, then this means
        // our Q_lower curve is mild.
        else
        {
            // If Q_lower is mild and Q_upper is steep, then we are in
            // a transitional region.
            if (steepC2)
            {
                double upstream;
                upstream = (c1firstp.y + c2firstp.y) / 2.;
                result = upstream;
            }//end if (steepC2)

            // Else, if Q_lower and Q_upper are mild, then we do mild
            // interpolation.
            else
            {
                double upstream;

                // If our downstream is less than the first point downstream, then
                // use the upstream critical depth for the given flow.
                // If our downstream is less than the first point on the upper
                // bounding curve, then use the upstream critical depth.
                if (downstream < c1firstp.x || downstream < c2firstp.x)
                {
                    double flow1, flow2;
                    if (flow >= 0.0)
                    {
                        flow1 = impl->posFlows.at(curve);
                        flow2 = impl->posFlows.at(curve + 1);
                    }
                    else
                    {
                        flow1 = impl->advFlows.at(curve);
                        flow2 = impl->advFlows.at(curve + 1);
                    }
                    upstream = linearInterp(flow, flow1, c1firstp.y, flow2, c2firstp.y);
                }

                // If our downstream more than the downstream point for the last
                // point on the curve, then we do extrapolation instead of interpolation.
                else if (downstream > lastp.x)
                {
                    if (HPGFAILURE(status = standardExtrapolation(curve, flow, downstream, upstream)))
                    {
                        impl->errorCode = status;
                        return impl->errorCode;
                    }
                }

                // Else, we are on the curve and we do our standard interpolation.
                else
                {
                    if (HPGFAILURE(status = standardInterpolation(curve, flow, downstream, upstream, InterpValue::Interp_Upstream)))
                    {
                        impl->errorCode = status;
                        return impl->errorCode;
                    }
                }

                result = upstream;
            }//end else
        }//end else

        return S_OK;
    }

    //int Hpg::GetDownstreamExactFlow(double flow, double upstream, double& result)
    //{
    //    if (fabs(flow) < 0.00001)
    //    {
    //        result = upstream;
    //        return S_OK;
    //    }
    //    impl->errorCode = S_OK;
    //    unsigned int curveIndex = 99999;
    //    if (HPGFAILURE(impl->errorCode = FindMatchingFlowIndex(flow, curveIndex)))
    //    {
    //        return impl->errorCode;
    //    }
    //    point p1;
    //    if (HPGFAILURE(impl->errorCode = getFirstPointOnCurve(flow, curveIndex, p1)))
    //    {
    //        return impl->errorCode;
    //    }
    //    if (flow > 0)
    //    {
    //        if (upstream >= p1.y)
    //        {
    //            result = impl->SplPosDS_QUS[curveIndex](upstream);
    //        }
    //        else
    //        {
    //            return (impl->errorCode = hpg::err::InvalidFlow);
    //        }
    //    }
    //    else
    //    {
    //        if (upstream >= p1.y)
    //        {
    //            result = impl->SplAdvDS_QUS[curveIndex](upstream);
    //        }
    //        else
    //        {
    //            return (impl->errorCode = hpg::err::InvalidFlow);
    //        }
    //    }
    //    return S_OK;
    //}

    /// Get the upstream value given the downstream value and the flow.
    /// This will select the proper interpolation/extrapolation routine
    /// and perform the interpolation/extrapolation.
    int Hpg::InterpVolume(double flow, double downstream, double& result)
    {
        impl->errorCode = S_OK;

        // Get the Q_lower flow index
        unsigned int curve;
        int status = S_OK;
        if (HPGFAILURE(status = findLowerBracketingCurve(flow, curve)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // Get the first point on the Q_lower curve
        point c1firstp;
        if (HPGFAILURE(status = getFirstPointOnCurve(flow, curve, c1firstp)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // Get the first point on the Q_upper curve
        point c2firstp;
        if (HPGFAILURE(status = getFirstPointOnCurve(flow, curve+1, c2firstp)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // Get the last point on the Q_lower curve. This is used to determine
        // if we need to perform extrapolation or interpolation.
        point lastp;
        if (HPGFAILURE(status = getLastPointOnCurve(flow, curve, lastp)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // If our downstream is less than the first point on the upper and lower
		// bounding curves, then interpolate between the volumes.
        if (downstream < c1firstp.x || downstream < c2firstp.x)
        {
			double f1, f2;
			if (flow >= 0.0)
			{
				f1 = impl->posFlows.at(curve);
				f2 = impl->posFlows.at(curve+1);
			}
			else
			{
				f1 = impl->advFlows.at(curve);
				f2 = impl->advFlows.at(curve+1);
			}

			result = linearInterpQ(flow, f1, f2, c1firstp.v, c2firstp.v);
			return S_OK;
        }

        // If our downstream more than the downstream point for the last
        // point on the curve, then we do extrapolation instead of interpolation.
        else if (downstream > lastp.x)
        {
            if (HPGFAILURE(status = standardExtrapolation(curve, flow, downstream, result)))
            {
                impl->errorCode = status;
                return impl->errorCode;
            }
        }

        // Else, we are on the curve and we do our standard interpolation.
        else
        {
            if (HPGFAILURE(status = standardInterpolation(curve, flow, downstream, result, InterpValue::Interp_Volume))) // true for volumes
            {
                impl->errorCode = status;
                return impl->errorCode;
            }
        }

		return S_OK;
    }

    /// Get the upstream hf friction value given the downstream depth and the flow.
    /// This will select the proper interpolation/extrapolation routine
    /// and perform the interpolation/extrapolation.
    int Hpg::InterpHf(double flow, double downstream, double& result)
    {
        impl->errorCode = S_OK;

        // Get the Q_lower flow index
        unsigned int curve;
        int status = S_OK;
        if (HPGFAILURE(status = findLowerBracketingCurve(flow, curve)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // Get the first point on the Q_lower curve
        point c1firstp;
        if (HPGFAILURE(status = getFirstPointOnCurve(flow, curve, c1firstp)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // Get the first point on the Q_upper curve
        point c2firstp;
        if (HPGFAILURE(status = getFirstPointOnCurve(flow, curve+1, c2firstp)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // Get the last point on the Q_lower curve. This is used to determine
        // if we need to perform extrapolation or interpolation.
        point lastp;
        if (HPGFAILURE(status = getLastPointOnCurve(flow, curve, lastp)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        // If our downstream is less than the first point on the upper and lower
		// bounding curves, then interpolate between the hf.
        if (downstream < c1firstp.x || downstream < c2firstp.x)
        {
			double f1, f2;
			if (flow >= 0.0)
			{
				f1 = impl->posFlows.at(curve);
				f2 = impl->posFlows.at(curve+1);
			}
			else
			{
				f1 = impl->advFlows.at(curve);
				f2 = impl->advFlows.at(curve+1);
			}

			result = linearInterpQ(flow, f1, f2, c1firstp.hf, c2firstp.hf);
			return S_OK;
        }

        // If our downstream more than the downstream point for the last
        // point on the curve, then we do extrapolation instead of interpolation.
        else if (downstream > lastp.x)
        {
            if (HPGFAILURE(status = standardExtrapolation(curve, flow, downstream, result)))
            {
                impl->errorCode = status;
                return impl->errorCode;
            }
        }

        // Else, we are on the curve and we do our standard interpolation.
        else
        {
            if (HPGFAILURE(status = standardInterpolation(curve, flow, downstream, result, InterpValue::Interp_Hf)))
            {
                impl->errorCode = status;
                return impl->errorCode;
            }
        }

		return S_OK;
    }

    ///// Get the upstream value on the critical line for the given flow.
    //int Hpg::GetCritUpstream(double flow, double& result)
    //{
    //    impl->errorCode = S_OK;

    //    // Get the flow immediately below this flow.
    //    unsigned int curve;
    //    int status = S_OK;
    //    if (HPGFAILURE(status = findLowerBracketingCurve(flow, curve)))
    //    {
    //        impl->errorCode = status;
    //        return impl->errorCode;
    //    }

    //    point c1firstp;
    //    if (HPGFAILURE(status = getFirstPointOnCurve(flow, curve, c1firstp)))
    //    {
    //        impl->errorCode = status;
    //        return impl->errorCode;
    //    }

    //    point c2firstp;
    //    if (HPGFAILURE(status = getFirstPointOnCurve(flow, curve+1, c2firstp)))
    //    {
    //        impl->errorCode = status;
    //        return impl->errorCode;
    //    }

    //    // This checks if the Q_lower curve is steep.
    //    if (c1firstp.x > c1firstp.y)
    //    {
    //        // If the Q_lower and Q_upper curves are both steep, then
    //        // we use our spline to interpolate.
    //        if (c2firstp.x > c2firstp.y)
    //        {
    //            if (flow >= 0.0)
    //                status = gsl_spline_eval_e(SplPosCritUS_Q, flow, 0, &result);
    //            else
    //                status = gsl_spline_eval_e(SplAdvCritUS_Q, flow, 0, &result);
    //        }

    //        // Else, the Q_lower is steep and the Q_upper is mild. Then
    //        // we do a linear interpolation.
    //        else
    //        {
    //            double flow1 = 0.0, flow2 = 0.0;
    //            if (flow >= 0.0)
    //            {
    //                flow1 = posFlows.at(curve);
    //                flow2 = posFlows.at(curve+1);
    //            }
    //            else
    //            {
    //                flow1 = advFlows.at(curve);
    //                flow2 = advFlows.at(curve+1);
    //            }

    //            double m = (c2firstp.y - c1firstp.y) / (flow2 - flow1);
    //            double b = c2firstp.y - m * flow2;

    //            result = m * flow + b;
    //        }
    //    }

    //    // Else, if Q_lower is mild
    //    else
    //    {
    //        // If the Q_lower curve is mild and the Q_upper curve is steep,
    //        // then do a linear interpolation.
    //        if (c2firstp.x > c2firstp.y)
    //        {
    //            double flow1 = 0.0, flow2 = 0.0;
    //            if (flow >= 0.0)
    //            {
    //                flow1 = posFlows.at(curve);
    //                flow2 = posFlows.at(curve+1);
    //            }
    //            else
    //            {
    //                flow1 = advFlows.at(curve);
    //                flow2 = advFlows.at(curve+1);
    //            }

    //            double m = (c2firstp.y - c1firstp.y) / (flow2 - flow1);
    //            double b = c2firstp.y - m * flow2;

    //            result = m * flow + b;
    //        }

    //        // Else, if both curves are mild, then we use our spline to
    //        // interpolate.
    //        else
    //        {
    //            if (flow >= 0.0)
    //                status = gsl_spline_eval_e(SplPosCritUS_Q, flow, 0, &result);
    //            else
    //                status = gsl_spline_eval_e(SplAdvCritUS_Q, flow, 0, &result);
    //        }
    //    }

    //    if (status)
    //    {
    //        impl->errorCode = err::GenericInterpFailed;
    //        return impl->errorCode;
    //    }
    //    else
    //        return S_OK;
    //}


    ///// Get the downstream value on the critical line for the
    ///// given flow.
    //int Hpg::GetCritDownstream(double flow, double& result)
    //{
    //    impl->errorCode = S_OK;

    //    // Get the flow immediately below this flow.
    //    unsigned int curve;
    //    int status = S_OK;
    //    if (HPGFAILURE(status = findLowerBracketingCurve(flow, curve)))
    //    {
    //        impl->errorCode = status;
    //        return impl->errorCode;
    //    }

    //    point c1firstp;
    //    if (HPGFAILURE(status = getFirstPointOnCurve(flow, curve, c1firstp)))
    //    {
    //        impl->errorCode = status;
    //        return impl->errorCode;
    //    }

    //    point c2firstp;
    //    if (HPGFAILURE(status = getFirstPointOnCurve(flow, curve+1, c2firstp)))
    //    {
    //        impl->errorCode = status;
    //        return impl->errorCode;
    //    }

    //    // This checks if the Q_lower curve is steep.
    //    if (c1firstp.x > c1firstp.y)
    //    {
    //        // If the Q_lower and Q_upper curves are both steep, then
    //        // we use our spline to interpolate.
    //        if (c2firstp.x > c2firstp.y)
    //        {
    //            if (flow >= 0.0)
    //                status = gsl_spline_eval_e(SplPosCritDS_Q, flow, 0, &result);
    //            else
    //                status = gsl_spline_eval_e(SplAdvCritDS_Q, flow, 0, &result);
    //        }

    //        // Else, the Q_lower is steep and the Q_upper is mild. Then
    //        // we do a linear interpolation.
    //        else
    //        {
    //            double flow1 = 0.0, flow2 = 0.0;
    //            if (flow >= 0.0)
    //            {
    //                flow1 = posFlows.at(curve);
    //                flow2 = posFlows.at(curve+1);
    //            }
    //            else
    //            {
    //                flow1 = advFlows.at(curve);
    //                flow2 = advFlows.at(curve+1);
    //            }

    //            double m = (flow2 - flow1) / (c2firstp.x - c1firstp.x);
    //            double b = c2firstp.x - m * flow2;

    //            result = m * flow + b;
    //        }
    //    }

    //    // Else, if Q_lower is mild
    //    else
    //    {
    //        // If the Q_lower curve is mild and the Q_upper curve is steep,
    //        // then do a linear interpolation.
    //        if (c2firstp.x > c2firstp.y)
    //        {
    //            double flow1 = 0.0, flow2 = 0.0;
    //            if (flow >= 0.0)
    //            {
    //                flow1 = posFlows.at(curve);
    //                flow2 = posFlows.at(curve+1);
    //            }
    //            else
    //            {
    //                flow1 = advFlows.at(curve);
    //                flow2 = advFlows.at(curve+1);
    //            }

    //            double m = (flow2 - flow1) / (c2firstp.x - c1firstp.x);
    //            double b = c2firstp.x - m * flow2;

    //            result = m * flow + b;
    //        }

    //        // Else, if both curves are mild, then we use our spline to
    //        // interpolate.
    //        else
    //        {
    //            if (flow >= 0.0)
    //                status = gsl_spline_eval_e(SplPosCritDS_Q, flow, 0, &result);
    //            else
    //                status = gsl_spline_eval_e(SplAdvCritDS_Q, flow, 0, &result);
    //        }
    //    }

    //    if (status)
    //    {
    //        impl->errorCode = err::GenericInterpFailed;
    //        return impl->errorCode;
    //    }
    //    else
    //        return S_OK;
    //}


    ///// Get the upstream value on the critical line for the
    ///// given downstream value.
    //int Hpg::GetCritUpFromDown(double flow, double downstream, double& result)
    //{
    //    impl->errorCode = S_OK;
    //    /*======================================================
    //    The math behind this:

    //    y_up = f(y_down)

    //    y_up

    //    ^
    //    |
    //    |
    //    |                   +
    //    |             +
    //    |        + 
    //    y_up = f(y) >|----+o
    //    |  +  |
    //    |+    |
    //    +--------------------> y_down
    //    ^
    //    y_down = y

    //    ======================================================*/

    //    // If the flow isn't in the valid HPG flow ranges, then return
    //    // an error.
    //    if (! isValidFlow(flow))
    //    {
    //        impl->errorCode = err::InvalidFlow;
    //        return impl->errorCode;
    //    }

    //    int status = S_OK;

    //    if (flow >= 0.0)
    //    {
    //        // First find the spline that we want to interpolate on.
    //        int index = -1;
    //        bool isRev = false;
    //        for (unsigned int i = 0; i < SplPosCritUS_DS_ranges.size(); i++)
    //        {
    //            if (flow >= SplPosCritUS_DS_ranges.at(i).x && flow <= SplPosCritUS_DS_ranges.at(i).y)
    //            {
    //                index = i;
    //                if (! SplPosCritUS_DS_ranges.at(i).v_valid)
    //                    isRev = true;
    //                break;
    //            }
    //        }

    //        // Now interpolate on that spline.
    //        if (isRev)
    //            downstream = -downstream; // reverse the sign if we're on a x-decreasing spline
    //        status = gsl_spline_eval_e(SplPosCritUS_DS.at(index), downstream, 0, &result);
    //    }
    //    else
    //        status = gsl_spline_eval_e(SplAdvCritUS_DS, downstream, 0, &result);

    //    if (status)
    //    {
    //        impl->errorCode = err::GenericInterpFailed;
    //        return impl->errorCode;
    //    }
    //    else
    //        return S_OK;
    //}


    ///// Get the upstream value on the zero-flow line for the given
    ///// downstream value.
    //int Hpg::GetZeroUpstream(double downstream, double& result)
    //{
    //    result = downstream;
    //    return S_OK;
    //}


    ///// Get the downstream value on the zero-flow line for the
    ///// given upstream value.
    //int Hpg::GetZeroDownstream(double upstream, double& result)
    //{
    //    result = upstream;
    //    return S_OK;
    //}


    /// Do standard linear interpolation.  This is used for both
    /// mild, steep, and adverse slope curves.
    int Hpg::standardInterpolation(unsigned int curve, double flow, double input, double& result, InterpValue interpAction)
    {
        impl->errorCode = S_OK;
        int status = S_OK;

		double flow1, flow2;
		double upstream1, upstream2;

        if (flow >= 0.0)
		{
			flow1 = impl->posFlows.at(curve);
			flow2 = impl->posFlows.at(curve + 1);
		}
        else
		{
			flow1 = impl->advFlows.at(curve);
			flow2 = impl->advFlows.at(curve + 1);
		}

		if (interpAction == InterpValue::Interp_Hf)
		{
			if (flow >= 0.0)
			{
				upstream1 = impl->SplPosHf.at(curve)(input);
				upstream2 = impl->SplPosHf.at(curve+1)(input);
			}
			else
			{
				upstream1 = impl->SplAdvHf.at(curve)(input);
				upstream2 = impl->SplAdvHf.at(curve+1)(input);
			}
		}
		else if (interpAction == InterpValue::Interp_Volume)
		{
			if (flow >= 0.0)
			{
				upstream1 = impl->SplPosVol.at(curve)(input);
				upstream2 = impl->SplPosVol.at(curve+1)(input);
			}
			else
			{
				upstream1 = impl->SplAdvVol.at(curve)(input);
				upstream2 = impl->SplAdvVol.at(curve+1)(input);
			}
		}
		else if (interpAction == InterpValue::Interp_Upstream)
		{
			if (flow >= 0.0)
			{
                //if (input >= impl->dsInvert + impl->maxDepth)
                //{
                //}
                //else
                //{
                    upstream1 = impl->SplPosUS_QDS.at(curve)(input);
                    upstream2 = impl->SplPosUS_QDS.at(curve + 1)(input);
                //}
			}
			else
			{
				upstream1 = impl->SplAdvUS_QDS.at(curve)(input);
				upstream2 = impl->SplAdvUS_QDS.at(curve+1)(input);
			}
		}
		else if (interpAction == InterpValue::Interp_Downstream)
		{
			if (flow >= 0.0)
			{
				upstream1 = impl->SplPosDS_QUS.at(curve)(input);
				upstream2 = impl->SplPosDS_QUS.at(curve+1)(input);
			}
			else
			{
				upstream1 = impl->SplAdvDS_QUS.at(curve)(input);
				upstream2 = impl->SplAdvDS_QUS.at(curve+1)(input);
			}
		}

        // Interpolate linearly between the two curves.
        result = linearInterpQ(flow, flow1, flow2, upstream1, upstream2);

        return S_OK;
    }


	double Hpg::linearInterpQ(double flow, double f1, double f2, double y1, double y2)
	{
        // Compute the flow factor.  This is (flow - flow_c1)/(flow_c2 - flow_c1).
        double factor = (flow - f1) / (f2 - f1);

		return y1 + factor * fabs(y1 - y2);
	}


	double Hpg::linearInterp(double x, double x1, double y1, double x2, double y2)
	{
		double m = (y2 - y1) / (x2 - x1);
		return m * (x - x2) + y2;
	}


    /// Do interpolation in the steep region where the downstream
    /// is between the min downstream values for the curves bracketing
    /// the given flow.
    int Hpg::interpolateSteepSpecial(unsigned int curve, double flow, double downstream, double& result)
    {
        impl->errorCode = S_OK;
        int status = S_OK;

        /*
        double critDown;
        if (HPGFAILURE(status = GetCritDownstream(flow, critDown)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }

        double critUp;
        if (HPGFAILURE(status = GetCritUpstream(flow, critUp)))
        {
            impl->errorCode = status;
            return impl->errorCode;
        }
        */

        point p1 = impl->posCritical.at(curve);
        point p2 = impl->posCritical.at(curve+1);

        // Get the point on the c-line that corresponds to the input downstream depth.
        //double pointOnCline = (p2.y - critUp) / (p2.x - critDown) * (downstream - critDown) + critUp;

        // Linearly interpolate on the c-line between the first points on the lower
        // and upper bounding curves.
        double yDonCline = (p2.y - p1.y) / (p2.x - p1.x) * (downstream - p1.x) + p1.y;

        /*
          Another method, but I think it underestimates the value: -- NOO
            double b = p2.x - critDown;
            double c = yDonCline - critDown;
            double a = p2.y - critUp;
            result = critUp + c / b * a;
        */

        // Evaluate the lower bounding spline at the downstream depth to get the
        // upstream depth on that spline.
        double upstream;
        if (flow > 0.0)
            upstream = impl->SplPosUS_QDS.at(curve)(downstream);
        else
            upstream = impl->SplAdvUS_QDS.at(curve)(downstream);

        if (status)
        {
            impl->errorCode = err::GenericInterpFailed;
            return impl->errorCode;
        }

        // Now say that the upstream point that we want is half-way between the
        // point on the c-line and the upstream value on the lower bounding curve.
        result = (upstream + yDonCline) * 0.5;

        return S_OK;
    }


    /// Do standard extrapolation.  This extrapolates the HPG
    /// for the given flow to the downstream value and gets the
    /// upstream value.
    int Hpg::standardExtrapolation(unsigned int curve, double flow, double downstream, double& result)
    {
		int status = S_OK;

		// Get the last point on the curves.
		point lastp1;
		if (HPGFAILURE(status = getLastPointOnCurve(flow, curve, lastp1)))
		{
			impl->errorCode = status;
			return impl->errorCode;
		}

		point lastp2;
		if (HPGFAILURE(status = getLastPointOnCurve(flow, curve+1, lastp2)))
		{
			impl->errorCode = status;
			return impl->errorCode;
		}

		// Get the flows for each curve.
		double f1, f2;
		if (flow > 0.0)
		{
			f1 = impl->posFlows.at(curve);
			f2 = impl->posFlows.at(curve+1);
		}
		else
		{
			f1 = impl->advFlows.at(curve);
			f2 = impl->advFlows.at(curve+1);
		}

		result = linearInterpQ(flow, f1, f2, lastp1.y, lastp2.y);

		return S_OK;
    }
}
