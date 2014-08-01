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
	int Hpg::findLowerBracketingCurve(double flow, unsigned int& index)
	{
		impl->errorCode = S_OK;
        index = 0;

		// If the requested flow to bracket is out of the
		// flow range, return an invalid index and set the
		// error code.
		if (! isValidFlow(flow))
			// We want to assign the error code as well as return it.
			return (impl->errorCode = err::InvalidFlow);

		// We do different things depending on the direction of the flow.
		if (flow >= 0.0)
		{
			// Look for the flow immediately less than the given input flow.
			for (unsigned int i = 0; i < impl->posFlows.size() && impl->posFlows.at(i) < flow; i++)
				index = i;

			// Check for null splines, in which case we can't interpolate.
			// If we have a null spline, we return an error.
			if (impl->SplPosUS_QDS.at(index).size() == 0 || impl->SplPosUS_QDS.at(index+1).size() == 0)
				// We want to assign the error code as well as return it.
				return (impl->errorCode = err::InvalidFlow);
		}
		else
		{
			// Look for the flow immediately less than the given input flow.
			for (unsigned int i = 0; i < impl->advFlows.size() && impl->advFlows.at(i) > flow; i++)
				index = i;

			// Check for null splines, in which case we can't interpolate.
			// If we have a null spline, we return an error.
			if (impl->SplAdvUS_QDS.at(index).size() == 0 || impl->SplAdvUS_QDS.at(index+1).size() == 0)
				// We want to assign the error code as well as return it.
				return (impl->errorCode = err::InvalidFlow);
		}

		return S_OK;
	}

	/////
	///// Get the curve index that is equal to the 
	///// requested flow.  Returns the status of the request.
	///// If an invalid flow is input, the output value is UINT_MAX.
	/////
	//int Hpg::FindMatchingFlowIndex(double flow, unsigned int& index)
	//{
	//	impl->errorCode = S_OK;
	//	index = UINT_MAX;

	//	// If the requested flow to bracket is out of the
	//	// flow range, return an invalid index and set the
	//	// error code.
	//	if (! isValidFlow(flow))
	//		// We want to assign the error code as well as return it.
	//		return (impl->errorCode = err::InvalidFlow);

	//	// We do different things depending on the direction of the flow.
	//	if (flow >= 0.0)
	//	{
 //           unsigned int i = 0;
 //           while (i < impl->posFlows.size() && fabs(flow - impl->posFlows.at(i)) > 0.00001)
 //           {
 //               i++;
 //           }

 //           if (i == impl->posFlows.size())
 //           {
 //               return (impl->errorCode = err::InvalidFlow);
 //           }
 //           else
 //           {
 //               index = i;
 //           }
	//	}
	//	else
	//	{
 //           unsigned int i = 0;
 //           while (i < impl->advFlows.size() && fabs(flow - impl->advFlows.at(i)) > 0.00001)
 //           {
 //               i++;
 //           }

 //           if (i == impl->advFlows.size())
 //           {
 //               return (impl->errorCode = err::InvalidFlow);
 //           }
 //           else
 //           {
 //               index = i;
 //           }
	//	}

	//	return S_OK;
	//}

	///
	/// Say if the given flow is in the valid range for this HPG.
	/// true = OK, false = invalid.
	///
	bool Hpg::isValidFlow(double flow)
	{
		bool ok = false;
		if (isValidFlowExtended(flow) == 0)
			ok = true;
		return ok;
	}

	///
	/// Say if the given flow is in the valid range for this HPG.
	/// 0 = OK, -1 = too low, +1 = too high.
	///
	int Hpg::isValidFlowExtended(double flow)
	{
		int ok = 0;
		if (flow >= 0.0)
		{
			if (flow > impl->maxPosFlow)
				ok = 1;
			else if (flow < impl->minPosFlow)
				ok = -1;
			else
			{
				ok = 0;
				int index = -1;
				// Look for the flow immediately less than the given input flow.
				for (int i = 0; i < (int)impl->posFlows.size() && impl->posFlows.at(i) < flow; i++)
					index = i;

                if (index == -1 && fabs(flow - impl->posFlows.at(0)) < 0.0001)
                {
                    index = 0;
                }

				// Check for null splines, in which case we can't interpolate.
				// If we have a null spline, we return an error.
				if (index < 0 || impl->SplPosUS_QDS.at(index).size() == 0 || impl->SplPosUS_QDS.at(index+1).size() == 0 || impl->posValues.at(index).size() < 4 || impl->posValues.at(index+1).size() < 4)
					// We want to assign the error code as well as return it.
					ok = -1;
			}
		}
		else
		{
			if (flow < impl->maxAdvFlow)
				ok = 1;
			else if (flow > impl->minAdvFlow)
				ok = -1;
			else
			{
				ok = 0;
				int index = -1;
				// Look for the flow immediately less than the given input flow.
				for (int i = 0; i < (int)impl->advFlows.size() && impl->advFlows.at(i) > flow; i++)
					index = i;

                if (index == -1 && fabs(flow - impl->advFlows.at(0)) < 0.0001)
                {
                    index = 0;
                }

				// Check for null splines, in which case we can't interpolate.
				// If we have a null spline, we return an error.
				if (index < 0 || impl->SplAdvUS_QDS.at(index).size() == 0 || impl->SplAdvUS_QDS.at(index+1).size() == 0 || impl->advValues.at(index).size() < 4 || impl->advValues.at(index+1).size() < 4)
					// We want to assign the error code as well as return it.
					ok = -1;
			}
		}
		return ok;
	}

	/////
	///// Get the last point for the given flow.  This is interpolated as
	///// necessary.
	/////
	//int Hpg::GetLastPoint(double flow, point& result)
	//{
	//	impl->errorCode = S_OK;
	//	result = NULL_POINT;
	//	unsigned int curve = UINT_MAX;
	//	int status = 0;
	//	if (HPGFAILURE(status = findLowerBracketingCurve(flow, curve)) || curve == UINT_MAX)
	//		return status;

	//	// Get the points on the curves for the flows immediately
	//	// below and above the given flow.
	//	point p1, p2;
	//	if (flow >= 0.0)
	//		p1 = impl->posValues.at(curve).back();
	//	else
	//		p1 = impl->advValues.at(curve).back();
	//	if (flow >= 0.0)
	//		p2 = impl->posValues.at(curve+1).back();
	//	else
	//		p2 = impl->advValues.at(curve+1).back();

	//	// Compute the discharge factor.  This is a factor used to scale
	//	// the difference between the y_up for the curves below and above.
	//	double factor;
	//	if (flow >= 0.0)
	//		factor = (flow - impl->posFlows.at(curve)) / (impl->posFlows.at(curve + 1) - impl->posFlows.at(curve));
	//	else
	//		factor = (flow - impl->advFlows.at(curve)) / (impl->advFlows.at(curve + 1) - impl->advFlows.at(curve));

	//	// If this is steep slope, do one thing.  If it is mild slope do another.
	//	if (p1.x < p2.x)
	//	{
	//		if (flow >= 0.0)
	//			p2.y = impl->SplPosUS_QDS.at(curve+1)(p1.x);
	//			//status = gsl_spline_eval_e(SplPosUS_QDS.at(curve+1), p1.x, NULL, &(p2.y));
	//		else
	//			p2.y = impl->SplAdvUS_QDS.at(curve+1)(p1.x);
	//			//status = gsl_spline_eval_e(SplAdvUS_QDS.at(curve+1), p1.x, NULL, &(p2.y));
	//		p2.x = p1.x;
	//	}
	//	else if (p2.x < p1.x)
	//	{
	//		if (flow >= 0.0)
	//			p1.y = impl->SplPosUS_QDS.at(curve)(p2.x);
	//			//status = gsl_spline_eval_e(SplPosUS_QDS.at(curve), p2.x, NULL, &(p1.y));
	//		else
	//			p1.y = impl->SplAdvUS_QDS.at(curve)(p2.x);
	//			//status = gsl_spline_eval_e(SplAdvUS_QDS.at(curve), p2.x, NULL, &(p1.y));
	//		p1.x = p2.x;
	//	}

	//	if (status)
	//		return (impl->errorCode = err::GenericInterpFailed);

	//	result = point(p1.x, p1.y + factor * fabs(p2.y - p1.y));

	//	return S_OK;
	//}

	///
	/// Get the first point on the curve with the given index.
	///
	int Hpg::getFirstPointOnCurve(double flow, unsigned int curve, point& result)
	{
		impl->errorCode = S_OK;

		if (flow >= 0.0) // Is a positive flow
		{
			if (curve >= impl->posFlowCount)
				// We want to assign the error code as well as return it.
				return (impl->errorCode = err::InvalidParam);
			result = impl->posValues.at(curve).at(0);
		}
		else
		{
			if (curve >= impl->advFlowCount)
				// We want to assign the error code as well as return it.
				return (impl->errorCode = err::InvalidParam);
			result = impl->advValues.at(curve).at(0);
		}

		return S_OK;
	}

	///
	/// Get the first point on the curve with the given index.
	///
	int Hpg::getLastPointOnCurve(double flow, unsigned int curve, point& result)
	{
		impl->errorCode = S_OK;

		if (flow >= 0.0) // Is a positive flow
		{
			if (curve >= impl->posFlowCount)
				// We want to assign the error code as well as return it.
				return (impl->errorCode = err::InvalidParam);
			result = impl->posValues.at(curve).back();
		}
		else
		{
			if (curve >= impl->advFlowCount)
				// We want to assign the error code as well as return it.
				return (impl->errorCode = err::InvalidParam);
			result = impl->advValues.at(curve).back();
		}

		return S_OK;
	}

}
