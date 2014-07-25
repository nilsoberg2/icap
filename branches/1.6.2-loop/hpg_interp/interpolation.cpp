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
    int Hpg::GetUpstream(double flow, double downstream, double& result)
    {
        // If there is no flow, then we return the downstream value,
        // as we are on the no-flow (Z-line).
        if (! flow)
        {
            result = downstream;
            return S_OK;
        }

        impl->ErrorCode = S_OK;

        // Get the Q_lower flow index
        unsigned int curve;
        int status = S_OK;
        if (HPGFAILURE(status = GetFlowIndex(flow, curve)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // Get the first point on the Q_lower curve
        point c1firstp;
        if (HPGFAILURE(status = GetFirstPointOnCurve(flow, curve, c1firstp)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // Get the first point on the Q_upper curve
        point c2firstp;
        if (HPGFAILURE(status = GetFirstPointOnCurve(flow, curve+1, c2firstp)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // Get the last point on the Q_lower curve. This is used to determine
        // if we need to perform extrapolation or interpolation.
        point lastp;
        if (HPGFAILURE(status = GetLastPointOnCurve(flow, curve, lastp)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // steepness from IsSteepAt: negative = steep, positive = mild, 0 = error
        int steepC1 = 1;
        int steepC2 = 1;

        // Make sure that the flow is positive before computing the steepness.
        if (flow > 0.0)
        {
            steepC1 = IsSteepAt(curve);
            steepC2 = IsSteepAt(curve+1);
        }
        
        // Check for an error in the steepness determination
        if (steepC1 == 0 || steepC2 == 0)
        {
            //impl->ErrorCode = err::CantComputeNormCrit; // errorcode is set by IsSteepAt
            return impl->ErrorCode;
        }

        // This tests to see if Q_lower is steep.
        if (steepC1 < 0)
        {
            // If Q_lower and Q_upper are steep, then we do steep interpolation.
            if (steepC2 < 0)
            {
                // Get the downstream critical value for the flow
                double critDown = -99999.0;
//TODO                if (HPGFAILURE(status = GetCritDownstream(flow, critDown)))
                {
                    impl->ErrorCode = status;
                    return impl->ErrorCode;
                }

                double upstream;

                // If our downstream point is less than the downstream critical
                // value for our flow, then we use the upstream critical value
                // for the result.
                if (downstream < critDown || downstream < c1firstp.x)
                {
					double flow1, flow2;
					if (flow > 0.0)
					{
						flow1 = impl->PosFlows.at(curve);
						flow2 = impl->PosFlows.at(curve + 1);
					}
					else
					{
						flow1 = impl->AdvFlows.at(curve);
						flow2 = impl->AdvFlows.at(curve + 1);
					}
					upstream = linearInterp(flow, flow1, c1firstp.y, flow2, c2firstp.y);
                }

                // Else, if our downstream is less than the first point (critical)
                // on the Q_upper curve, then we do a special type of interpolation.
                else if (downstream < c2firstp.x)
                {
                    if (HPGFAILURE(status = interpolateSteepSpecial(curve, flow, downstream, upstream)))
                    {
                        impl->ErrorCode = status;
                        return impl->ErrorCode;
                    }
                }

                // Else, if we are more than the last downstream point on the curve,
                // then we extrapolate.
                else if (downstream > lastp.x)
                {
                    if (HPGFAILURE(status = standardExtrapolation(curve, flow, downstream, upstream)))
                    {
                        impl->ErrorCode = status;
                        return impl->ErrorCode;
                    }
                }

                // Otherwise, we do standard interpolation.
                else
                {
                    if (HPGFAILURE(status = standardInterpolation(curve, flow, downstream, upstream, InterpValue::Interp_Upstream)))
                    {
                        impl->ErrorCode = status;
                        return impl->ErrorCode;
                    }
                }

                // Save the result.
                result = upstream;

            }//end if (steepC2 < 0)

            // Else, if Q_lower is steep and Q_upper is mild, then we are
            // in a transitional region.
            else
            {
                double upstream;
//TODO:                //if (HPGFAILURE(status = GetCritUpstream(flow, upstream)))
                {
                    impl->ErrorCode = status;
                    return impl->ErrorCode;
                }
                result = upstream;
            }//end else
        }//end if (steepC1 < 0)

        // If the matching if conditional is false, then this means
        // our Q_lower curve is mild.
        else
        {
            // If Q_lower is mild and Q_upper is steep, then we are in
            // a transitional region.
            if (steepC2 < 0)
            {
                double upstream;
//TODO:                if (HPGFAILURE(status = GetCritUpstream(flow, upstream)))
                {
                    impl->ErrorCode = status;
                    return impl->ErrorCode;
                }
                result = upstream;
            }//end if (steepC2 < 0)

            // Else, if Q_lower and Q_upper are mild, then we do mild
            // interpolation.
            else
            {
                // Get the downstream critical value for the flow
                double critDown = -99999.0;
//TODO:                if (HPGFAILURE(status = GetCritDownstream(flow, critDown)))
                {
                    impl->ErrorCode = status;
                    return impl->ErrorCode;
                }

                double upstream;

                // If our downstream is less than the first point downstream, then
                // use the upstream critical depth for the given flow.
                // If our downstream is less than the first point on the upper
                // bounding curve, then use the upstream critical depth.
                if (downstream < c1firstp.x || downstream < c2firstp.x)
                {
					double flow1, flow2;
					if (flow > 0.0)
					{
						flow1 = impl->PosFlows.at(curve);
						flow2 = impl->PosFlows.at(curve + 1);
					}
					else
					{
						flow1 = impl->AdvFlows.at(curve);
						flow2 = impl->AdvFlows.at(curve + 1);
					}
					upstream = linearInterp(flow, flow1, c1firstp.y, flow2, c2firstp.y);
                }

                // If our downstream more than the downstream point for the last
                // point on the curve, then we do extrapolation instead of interpolation.
                else if (downstream > lastp.x)
                {
                    if (HPGFAILURE(status = standardExtrapolation(curve, flow, downstream, upstream)))
                    {
                        impl->ErrorCode = status;
                        return impl->ErrorCode;
                    }
                }

                // Else, we are on the curve and we do our standard interpolation.
                else
                {
                    if (HPGFAILURE(status = standardInterpolation(curve, flow, downstream, upstream, InterpValue::Interp_Upstream)))
                    {
                        impl->ErrorCode = status;
                        return impl->ErrorCode;
                    }
                }

                result = upstream;
            }//end else
        }//end else

        return S_OK;
    }
    
    int Hpg::GetDownstreamExactFlow(double flow, double upstream, double& result)
    {
        if (fabs(flow) < 0.00001)
        {
            result = upstream;
            return S_OK;
        }

        impl->ErrorCode = S_OK;

        unsigned int curveIndex = 99999;
        if (HPGFAILURE(impl->ErrorCode = FindMatchingFlowIndex(flow, curveIndex)))
        {
            return impl->ErrorCode;
        }

        point p1;
        if (HPGFAILURE(impl->ErrorCode = GetFirstPointOnCurve(flow, curveIndex, p1)))
        {
            return impl->ErrorCode;
        }

        if (flow > 0)
        {
            if (upstream >= p1.y)
            {
                result = impl->SplPosDS_QUS[curveIndex](upstream);
            }
            else
            {
                return (impl->ErrorCode = hpg::err::InvalidFlow);
            }
        }
        else
        {
            if (upstream >= p1.y)
            {
                result = impl->SplAdvDS_QUS[curveIndex](upstream);
            }
            else
            {
                return (impl->ErrorCode = hpg::err::InvalidFlow);
            }
        }

        return S_OK;
    }

    /// Get the upstream value given the downstream value and the flow.
    /// This will select the proper interpolation/extrapolation routine
    /// and perform the interpolation/extrapolation.
    int Hpg::GetVolume(double flow, double downstream, double& result)
    {
        // If there is no flow, then we return the downstream value,
        // as we are on the no-flow (Z-line).
        if (! flow)
        {
            result = 0.0;
            return S_OK;
        }

        impl->ErrorCode = S_OK;

        // Get the Q_lower flow index
        unsigned int curve;
        int status = S_OK;
        if (HPGFAILURE(status = GetFlowIndex(flow, curve)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // Get the first point on the Q_lower curve
        point c1firstp;
        if (HPGFAILURE(status = GetFirstPointOnCurve(flow, curve, c1firstp)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // Get the first point on the Q_upper curve
        point c2firstp;
        if (HPGFAILURE(status = GetFirstPointOnCurve(flow, curve+1, c2firstp)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // Get the last point on the Q_lower curve. This is used to determine
        // if we need to perform extrapolation or interpolation.
        point lastp;
        if (HPGFAILURE(status = GetLastPointOnCurve(flow, curve, lastp)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // If our downstream is less than the first point on the upper and lower
		// bounding curves, then interpolate between the volumes.
        if (downstream < c1firstp.x || downstream < c2firstp.x)
        {
			double f1, f2;
			if (flow > 0.0)
			{
				f1 = impl->PosFlows.at(curve);
				f2 = impl->PosFlows.at(curve+1);
			}
			else
			{
				f1 = impl->AdvFlows.at(curve);
				f2 = impl->AdvFlows.at(curve+1);
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
                impl->ErrorCode = status;
                return impl->ErrorCode;
            }
        }

        // Else, we are on the curve and we do our standard interpolation.
        else
        {
            if (HPGFAILURE(status = standardInterpolation(curve, flow, downstream, result, InterpValue::Interp_Volume))) // true for volumes
            {
                impl->ErrorCode = status;
                return impl->ErrorCode;
            }
        }

		return S_OK;
    }

    /// Get the upstream hf friction value given the downstream depth and the flow.
    /// This will select the proper interpolation/extrapolation routine
    /// and perform the interpolation/extrapolation.
    int Hpg::GetHf(double flow, double downstream, double& result)
    {
        // If there is no flow, then we return the downstream value,
        // as we are on the no-flow (Z-line).
        if (! flow)
        {
            result = 0.0;
            return S_OK;
        }

        impl->ErrorCode = S_OK;

        // Get the Q_lower flow index
        unsigned int curve;
        int status = S_OK;
        if (HPGFAILURE(status = GetFlowIndex(flow, curve)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // Get the first point on the Q_lower curve
        point c1firstp;
        if (HPGFAILURE(status = GetFirstPointOnCurve(flow, curve, c1firstp)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // Get the first point on the Q_upper curve
        point c2firstp;
        if (HPGFAILURE(status = GetFirstPointOnCurve(flow, curve+1, c2firstp)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // Get the last point on the Q_lower curve. This is used to determine
        // if we need to perform extrapolation or interpolation.
        point lastp;
        if (HPGFAILURE(status = GetLastPointOnCurve(flow, curve, lastp)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        // If our downstream is less than the first point on the upper and lower
		// bounding curves, then interpolate between the hf.
        if (downstream < c1firstp.x || downstream < c2firstp.x)
        {
			double f1, f2;
			if (flow > 0.0)
			{
				f1 = impl->PosFlows.at(curve);
				f2 = impl->PosFlows.at(curve+1);
			}
			else
			{
				f1 = impl->AdvFlows.at(curve);
				f2 = impl->AdvFlows.at(curve+1);
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
                impl->ErrorCode = status;
                return impl->ErrorCode;
            }
        }

        // Else, we are on the curve and we do our standard interpolation.
        else
        {
            if (HPGFAILURE(status = standardInterpolation(curve, flow, downstream, result, InterpValue::Interp_Hf)))
            {
                impl->ErrorCode = status;
                return impl->ErrorCode;
            }
        }

		return S_OK;
    }

    ///// Get the upstream value on the critical line for the given flow.
    //int Hpg::GetCritUpstream(double flow, double& result)
    //{
    //    impl->ErrorCode = S_OK;

    //    // Get the flow immediately below this flow.
    //    unsigned int curve;
    //    int status = S_OK;
    //    if (HPGFAILURE(status = GetFlowIndex(flow, curve)))
    //    {
    //        impl->ErrorCode = status;
    //        return impl->ErrorCode;
    //    }

    //    point c1firstp;
    //    if (HPGFAILURE(status = GetFirstPointOnCurve(flow, curve, c1firstp)))
    //    {
    //        impl->ErrorCode = status;
    //        return impl->ErrorCode;
    //    }

    //    point c2firstp;
    //    if (HPGFAILURE(status = GetFirstPointOnCurve(flow, curve+1, c2firstp)))
    //    {
    //        impl->ErrorCode = status;
    //        return impl->ErrorCode;
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
    //                flow1 = PosFlows.at(curve);
    //                flow2 = PosFlows.at(curve+1);
    //            }
    //            else
    //            {
    //                flow1 = AdvFlows.at(curve);
    //                flow2 = AdvFlows.at(curve+1);
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
    //                flow1 = PosFlows.at(curve);
    //                flow2 = PosFlows.at(curve+1);
    //            }
    //            else
    //            {
    //                flow1 = AdvFlows.at(curve);
    //                flow2 = AdvFlows.at(curve+1);
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
    //        impl->ErrorCode = err::GenericInterpFailed;
    //        return impl->ErrorCode;
    //    }
    //    else
    //        return S_OK;
    //}


    ///// Get the downstream value on the critical line for the
    ///// given flow.
    //int Hpg::GetCritDownstream(double flow, double& result)
    //{
    //    impl->ErrorCode = S_OK;

    //    // Get the flow immediately below this flow.
    //    unsigned int curve;
    //    int status = S_OK;
    //    if (HPGFAILURE(status = GetFlowIndex(flow, curve)))
    //    {
    //        impl->ErrorCode = status;
    //        return impl->ErrorCode;
    //    }

    //    point c1firstp;
    //    if (HPGFAILURE(status = GetFirstPointOnCurve(flow, curve, c1firstp)))
    //    {
    //        impl->ErrorCode = status;
    //        return impl->ErrorCode;
    //    }

    //    point c2firstp;
    //    if (HPGFAILURE(status = GetFirstPointOnCurve(flow, curve+1, c2firstp)))
    //    {
    //        impl->ErrorCode = status;
    //        return impl->ErrorCode;
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
    //                flow1 = PosFlows.at(curve);
    //                flow2 = PosFlows.at(curve+1);
    //            }
    //            else
    //            {
    //                flow1 = AdvFlows.at(curve);
    //                flow2 = AdvFlows.at(curve+1);
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
    //                flow1 = PosFlows.at(curve);
    //                flow2 = PosFlows.at(curve+1);
    //            }
    //            else
    //            {
    //                flow1 = AdvFlows.at(curve);
    //                flow2 = AdvFlows.at(curve+1);
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
    //        impl->ErrorCode = err::GenericInterpFailed;
    //        return impl->ErrorCode;
    //    }
    //    else
    //        return S_OK;
    //}


    ///// Get the upstream value on the critical line for the
    ///// given downstream value.
    //int Hpg::GetCritUpFromDown(double flow, double downstream, double& result)
    //{
    //    impl->ErrorCode = S_OK;
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
    //    if (! IsValidFlow(flow))
    //    {
    //        impl->ErrorCode = err::InvalidFlow;
    //        return impl->ErrorCode;
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
    //        impl->ErrorCode = err::GenericInterpFailed;
    //        return impl->ErrorCode;
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
        impl->ErrorCode = S_OK;
        int status = S_OK;

		double flow1, flow2;
		double upstream1, upstream2;

        if (flow > 0.0)
		{
			flow1 = impl->PosFlows.at(curve);
			flow2 = impl->PosFlows.at(curve + 1);
		}
        else
		{
			flow1 = impl->AdvFlows.at(curve);
			flow2 = impl->AdvFlows.at(curve + 1);
		}

		if (interpAction == InterpValue::Interp_Hf)
		{
			if (flow > 0.0)
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
			if (flow > 0.0)
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
			if (flow > 0.0)
			{
				upstream1 = impl->SplPosUS_QDS.at(curve)(input);
				upstream2 = impl->SplPosUS_QDS.at(curve+1)(input);
			}
			else
			{
				upstream1 = impl->SplAdvUS_QDS.at(curve)(input);
				upstream2 = impl->SplAdvUS_QDS.at(curve+1)(input);
			}
		}
		else if (interpAction == InterpValue::Interp_Downstream)
		{
			if (flow > 0.0)
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
        impl->ErrorCode = S_OK;
        int status = S_OK;

        /*
        double critDown;
        if (HPGFAILURE(status = GetCritDownstream(flow, critDown)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }

        double critUp;
        if (HPGFAILURE(status = GetCritUpstream(flow, critUp)))
        {
            impl->ErrorCode = status;
            return impl->ErrorCode;
        }
        */

        point p1 = impl->PosCritical.at(curve);
        point p2 = impl->PosCritical.at(curve+1);

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
            impl->ErrorCode = err::GenericInterpFailed;
            return impl->ErrorCode;
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
		if (HPGFAILURE(status = GetLastPointOnCurve(flow, curve, lastp1)))
		{
			impl->ErrorCode = status;
			return impl->ErrorCode;
		}

		point lastp2;
		if (HPGFAILURE(status = GetLastPointOnCurve(flow, curve+1, lastp2)))
		{
			impl->ErrorCode = status;
			return impl->ErrorCode;
		}

		// Get the flows for each curve.
		double f1, f2;
		if (flow > 0.0)
		{
			f1 = impl->PosFlows.at(curve);
			f2 = impl->PosFlows.at(curve+1);
		}
		else
		{
			f1 = impl->AdvFlows.at(curve);
			f2 = impl->AdvFlows.at(curve+1);
		}

		result = linearInterpQ(flow, f1, f2, lastp1.y, lastp2.y);

		return S_OK;
    }
}
