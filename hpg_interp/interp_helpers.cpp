#include <limits.h>
#include <math.h>
#include "hpg.hpp"
#include "errors.hpp"
#include "impl.h"


namespace hpg
{

	///
	/// Get the curve index that is immediately less than the
	/// requested flow.  Returns the status of the request.
	/// If an invalid flow is input, the output value is .
	///
	int Hpg::GetFlowIndex(double flow, unsigned int& index)
	{
		impl->ErrorCode = S_OK;
		index = UINT_MAX;

		// If the requested flow to bracket is out of the
		// flow range, return an invalid index and set the
		// error code.
		if (! IsValidFlow(flow))
			// We want to assign the error code as well as return it.
			return (impl->ErrorCode = err::InvalidFlow);

		// We do different things depending on the direction of the flow.
		if (flow >= 0.0)
		{
			// Look for the flow immediately less than the given input flow.
			for (unsigned int i = 0; i < impl->PosFlows.size() && impl->PosFlows.at(i) < flow; i++)
				index = i;

			// Check for null splines, in which case we can't interpolate.
			// If we have a null spline, we return an error.
			if (impl->SplPosUS_QDS.at(index).size() == 0 || impl->SplPosUS_QDS.at(index+1).size() == 0)
				// We want to assign the error code as well as return it.
				return (impl->ErrorCode = err::InvalidFlow);
		}
		else
		{
			// Look for the flow immediately less than the given input flow.
			for (unsigned int i = 0; i < impl->AdvFlows.size() && impl->AdvFlows.at(i) > flow; i++)
				index = i;

			// Check for null splines, in which case we can't interpolate.
			// If we have a null spline, we return an error.
			if (impl->SplAdvUS_QDS.at(index).size() == 0 || impl->SplAdvUS_QDS.at(index+1).size() == 0)
				// We want to assign the error code as well as return it.
				return (impl->ErrorCode = err::InvalidFlow);
		}

		return S_OK;
	}

	///
	/// Get the curve index that is equal to the 
	/// requested flow.  Returns the status of the request.
	/// If an invalid flow is input, the output value is UINT_MAX.
	///
	int Hpg::FindMatchingFlowIndex(double flow, unsigned int& index)
	{
		impl->ErrorCode = S_OK;
		index = UINT_MAX;

		// If the requested flow to bracket is out of the
		// flow range, return an invalid index and set the
		// error code.
		if (! IsValidFlow(flow))
			// We want to assign the error code as well as return it.
			return (impl->ErrorCode = err::InvalidFlow);

		// We do different things depending on the direction of the flow.
		if (flow >= 0.0)
		{
            unsigned int i = 0;
            while (i < impl->PosFlows.size() && fabs(flow - impl->PosFlows.at(i)) > 0.00001)
            {
                i++;
            }

            if (i == impl->PosFlows.size())
            {
                return (impl->ErrorCode = err::InvalidFlow);
            }
            else
            {
                index = i;
            }
		}
		else
		{
            unsigned int i = 0;
            while (i < impl->AdvFlows.size() && fabs(flow - impl->AdvFlows.at(i)) > 0.00001)
            {
                i++;
            }

            if (i == impl->AdvFlows.size())
            {
                return (impl->ErrorCode = err::InvalidFlow);
            }
            else
            {
                index = i;
            }
		}

		return S_OK;
	}

	///
	/// Say if the given flow is in the valid range for this HPG.
	/// true = OK, false = invalid.
	///
	bool Hpg::IsValidFlow(double flow)
	{
		bool ok = false;
		if (IsValidFlowExtended(flow) == 0)
			ok = true;
		return ok;
	}

	///
	/// Say if the given flow is in the valid range for this HPG.
	/// 0 = OK, -1 = too low, +1 = too high.
	///
	int Hpg::IsValidFlowExtended(double flow)
	{
		int ok = 0;
		if (flow >= 0.0)
		{
			if (flow > impl->MaxPosFlow)
				ok = 1;
			else if (flow < impl->MinPosFlow)
				ok = -1;
			else
			{
				ok = 0;
				int index = -1;
				// Look for the flow immediately less than the given input flow.
				for (int i = 0; i < (int)impl->PosFlows.size() && impl->PosFlows.at(i) < flow; i++)
					index = i;

                if (index == -1 && fabs(flow - impl->PosFlows.at(0)) < 0.0001)
                {
                    index = 0;
                }

				// Check for null splines, in which case we can't interpolate.
				// If we have a null spline, we return an error.
				if (index < 0 || impl->SplPosUS_QDS.at(index).size() == 0 || impl->SplPosUS_QDS.at(index+1).size() == 0 || impl->PosValues.at(index).size() < 4 || impl->PosValues.at(index+1).size() < 4)
					// We want to assign the error code as well as return it.
					ok = -1;
			}
		}
		else
		{
			if (flow < impl->MaxAdvFlow)
				ok = 1;
			else if (flow > impl->MinAdvFlow)
				ok = -1;
			else
			{
				ok = 0;
				int index = -1;
				// Look for the flow immediately less than the given input flow.
				for (int i = 0; i < (int)impl->AdvFlows.size() && impl->AdvFlows.at(i) > flow; i++)
					index = i;

                if (index == -1 && fabs(flow - impl->AdvFlows.at(0)) < 0.0001)
                {
                    index = 0;
                }

				// Check for null splines, in which case we can't interpolate.
				// If we have a null spline, we return an error.
				if (index < 0 || impl->SplAdvUS_QDS.at(index).size() == 0 || impl->SplAdvUS_QDS.at(index+1).size() == 0 || impl->AdvValues.at(index).size() < 4 || impl->AdvValues.at(index+1).size() < 4)
					// We want to assign the error code as well as return it.
					ok = -1;
			}
		}
		return ok;
	}

	///
	/// Get the last point for the given flow.  This is interpolated as
	/// necessary.
	///
	int Hpg::GetLastPoint(double flow, point& result)
	{
		impl->ErrorCode = S_OK;
		result = NULL_POINT;
		unsigned int curve = UINT_MAX;
		int status = 0;
		if (HPGFAILURE(status = GetFlowIndex(flow, curve)) || curve == UINT_MAX)
			return status;

		// Get the points on the curves for the flows immediately
		// below and above the given flow.
		point p1, p2;
		if (flow >= 0.0)
			p1 = impl->PosValues.at(curve).back();
		else
			p1 = impl->AdvValues.at(curve).back();
		if (flow >= 0.0)
			p2 = impl->PosValues.at(curve+1).back();
		else
			p2 = impl->AdvValues.at(curve+1).back();

		// Compute the discharge factor.  This is a factor used to scale
		// the difference between the y_up for the curves below and above.
		double factor;
		if (flow >= 0.0)
			factor = (flow - impl->PosFlows.at(curve)) / (impl->PosFlows.at(curve + 1) - impl->PosFlows.at(curve));
		else
			factor = (flow - impl->AdvFlows.at(curve)) / (impl->AdvFlows.at(curve + 1) - impl->AdvFlows.at(curve));

		// If this is steep slope, do one thing.  If it is mild slope do another.
		if (p1.x < p2.x)
		{
			if (flow >= 0.0)
				p2.y = impl->SplPosUS_QDS.at(curve+1)(p1.x);
				//status = gsl_spline_eval_e(SplPosUS_QDS.at(curve+1), p1.x, NULL, &(p2.y));
			else
				p2.y = impl->SplAdvUS_QDS.at(curve+1)(p1.x);
				//status = gsl_spline_eval_e(SplAdvUS_QDS.at(curve+1), p1.x, NULL, &(p2.y));
			p2.x = p1.x;
		}
		else if (p2.x < p1.x)
		{
			if (flow >= 0.0)
				p1.y = impl->SplPosUS_QDS.at(curve)(p2.x);
				//status = gsl_spline_eval_e(SplPosUS_QDS.at(curve), p2.x, NULL, &(p1.y));
			else
				p1.y = impl->SplAdvUS_QDS.at(curve)(p2.x);
				//status = gsl_spline_eval_e(SplAdvUS_QDS.at(curve), p2.x, NULL, &(p1.y));
			p1.x = p2.x;
		}

		if (status)
			return (impl->ErrorCode = err::GenericInterpFailed);

		result = point(p1.x, p1.y + factor * fabs(p2.y - p1.y));

		return S_OK;
	}

	///
	/// Get the first point on the curve with the given index.
	///
	int Hpg::GetFirstPointOnCurve(double flow, unsigned int curve, point& result)
	{
		impl->ErrorCode = S_OK;

		if (flow > 0.0) // Is a positive flow
		{
			if (curve >= impl->PosFlowCount)
				// We want to assign the error code as well as return it.
				return (impl->ErrorCode = err::InvalidParam);
			result = impl->PosValues.at(curve).at(0);
		}
		else
		{
			if (curve >= impl->AdvFlowCount)
				// We want to assign the error code as well as return it.
				return (impl->ErrorCode = err::InvalidParam);
			result = impl->AdvValues.at(curve).at(0);
		}

		return S_OK;
	}

	///
	/// Get the first point on the curve with the given index.
	///
	int Hpg::GetLastPointOnCurve(double flow, unsigned int curve, point& result)
	{
		impl->ErrorCode = S_OK;

		if (flow > 0.0) // Is a positive flow
		{
			if (curve >= impl->PosFlowCount)
				// We want to assign the error code as well as return it.
				return (impl->ErrorCode = err::InvalidParam);
			result = impl->PosValues.at(curve).back();
		}
		else
		{
			if (curve >= impl->AdvFlowCount)
				// We want to assign the error code as well as return it.
				return (impl->ErrorCode = err::InvalidParam);
			result = impl->AdvValues.at(curve).back();
		}

		return S_OK;
	}

}
