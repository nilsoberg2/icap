
#include "hpg.hpp"
#include "errors.hpp"
#include "impl.h"


namespace hpg
{
    Hpg::~Hpg()
    {
    }

    unsigned int Hpg::NumPosFlows()
    {
        return impl->PosFlowCount;
    }

    unsigned int Hpg::NumAdvFlows()
    {
        return impl->AdvFlowCount;
    }

    double Hpg::PosFlowAt(unsigned int f)
    {
        return impl->PosFlows.at(f);
    }

    double Hpg::AdvFlowAt(unsigned int f)
    {
        return impl->AdvFlows.at(f);
    }

    hpgvec& Hpg::PosValuesAt(unsigned int f)
    {
        return impl->PosValues.at(f);
    }

    hpgvec& Hpg::AdvValuesAt(unsigned int f)
    {
        return impl->AdvValues.at(f);
    }

    bool Hpg::IsMildAt(unsigned int curve)
    {
        point p = impl->PosValues.at(curve).at(0);
        if (p.x <= p.y)
            return true;
        else
            return false;
    }

    void Hpg::AddCurve(double flow, hpgvec& curve, point crit)
    {
        if (flow > 0.0)
        {
            impl->PosFlowCount++;
            impl->PosFlows.push_back(flow);
            impl->PosValues.push_back(curve);
            impl->PosCritical.push_back(crit);
            if (flow < impl->MinPosFlow)
                impl->MinPosFlow = flow;
            if (flow > impl->MaxPosFlow)
                impl->MaxPosFlow = flow;
        }
        else
        {
            impl->AdvFlowCount++;
            impl->AdvFlows.push_back(flow);
            impl->AdvValues.push_back(curve);
            impl->AdvCritical.push_back(crit);
            if (flow > impl->MinAdvFlow)
                impl->MinAdvFlow = flow;
            if (flow < impl->MaxAdvFlow)
                impl->MaxAdvFlow = flow;
        }
    }

    hpgvec& Hpg::GetPosCritical()
    {
        return impl->PosCritical;
    }

    hpgvec& Hpg::GetAdvCritical()
    {
        return impl->AdvCritical;
    }

    void Hpg::Deinitialize()
    {
        impl->SplPosCritUS_DS.clear();
        impl->SplPosCritUS_DS_ranges.clear();
    }

    void Hpg::Initialize()
    {
        impl->ErrorCode = S_OK;
        impl->MinPosFlow = 1000000.0;
        impl->MaxPosFlow = 0.0;
        impl->MaxAdvFlow = 0.0;
        impl->MinAdvFlow = -1000000.0;
        //    MinPosFlow = MaxPosFlow = MinAdvFlow = MaxAdvFlow = 0.0;
        impl->PosFlowCount = impl->AdvFlowCount = 0;

        impl->dsInvertValid = impl->usInvertValid = impl->dsStationValid = impl->usStationValid =
            impl->slopeValid = impl->lengthValid = impl->roughnessValid = impl->maxDepthValid = impl->unsteadyDepthPctValid = false;
        impl->dsInvert = impl->usInvert = impl->dsStation = impl->usStation = impl->slope =
            impl->length = impl->roughness = impl->maxDepth = impl->unsteadyDepthPct = 0.0;
        impl->nodeID = -1;
    }

    void Hpg::CopyInto(const Hpg& copy)
    {
        impl->CopyInto(copy.impl);

        // Erase all the splines.  We recreate them in setupSplines below.
        //for (unsigned int i = 0; i < SplPosUS_QDS.size(); i++)
        //    if (SplPosUS_QDS.at(i) != NULL)
        //        gsl_spline_free(SplPosUS_QDS.at(i));
        impl->SplPosUS_QDS.clear();

        //for (unsigned int i = 0; i < SplAdvUS_QDS.size(); i++)
        //    if (SplAdvUS_QDS.at(i) != NULL)
        //        gsl_spline_free(SplAdvUS_QDS.at(i));
        impl->SplAdvUS_QDS.clear();

        impl->SplPosDS_QUS.clear();
        impl->SplAdvDS_QUS.clear();

        //for (unsigned int i = 0; i < SplPosVol.size(); i++)
        //    if (SplPosVol.at(i) != NULL)
        //        gsl_spline_free(SplPosVol.at(i));
		impl->SplPosVol.clear();

		impl->SplPosHf.clear();

        //for (unsigned int i = 0; i < SplAdvVol.size(); i++)
        //    if (SplAdvVol.at(i) != NULL)
        //        gsl_spline_free(SplAdvVol.at(i));
		impl->SplAdvVol.clear();

		impl->SplAdvHf.clear();

        //for (unsigned int i = 0; i < SplPosCritUS_DS.size(); i++)
        //    if (SplPosCritUS_DS.at(i) != NULL)
        //        gsl_spline_free(SplPosCritUS_DS.at(i));
        impl->SplPosCritUS_DS.clear();
        impl->SplPosCritUS_DS_ranges.clear();

        //if (SplPosCritDS_Q != NULL)
        //    gsl_spline_free(SplPosCritDS_Q);
        //if (SplPosCritUS_Q != NULL)
        //    gsl_spline_free(SplPosCritUS_Q);
        //if (SplAdvCritUS_DS != NULL)
        //    gsl_spline_free(SplAdvCritUS_DS);
        //if (SplAdvCritDS_Q != NULL)
        //    gsl_spline_free(SplAdvCritDS_Q);
        //if (SplAdvCritUS_Q != NULL)
        //    gsl_spline_free(SplAdvCritUS_Q);

        if (impl->PosFlowCount || impl->AdvFlowCount)
            SetupSplines();
    }

    bool Hpg::IsDSInvertValid()
    {
        return impl->dsInvertValid;
    }

    double Hpg::GetDSInvert()
    {
        return impl->dsInvert;
    }

    void Hpg::SetDSInvert(double dsinvert)
    {
        impl->dsInvert = dsinvert;
        impl->dsInvertValid = true;
    }

    int Hpg::GetNodeID()
    {
        return impl->nodeID;
    }

    void Hpg::SetNodeID(int id)
    {
        impl->nodeID = id;
    }

    bool Hpg::IsUSInvertValid()
    {
        return impl->usInvertValid;
    }

    double Hpg::GetUSInvert()
    {
        return impl->usInvert;
    }

    void Hpg::SetUSInvert(double usinvert)
    {
        impl->usInvert = usinvert;
        impl->usInvertValid = true;
    }

    bool Hpg::IsDSStationValid()
    {
        return impl->dsStationValid;
    }

    double Hpg::GetDSStation()
    {
        return impl->dsStation;
    }

    void Hpg::SetDSStation(double dsstation)
    {
        impl->dsStation = dsstation;
        impl->dsStationValid = true;
    }

    bool Hpg::IsUSStationValid()
    {
        return impl->usStationValid;
    }

    double Hpg::GetUSStation()
    {
        return impl->usStation;
    }

    void Hpg::SetUSStation(double usstation)
    {
        impl->usStation = usstation;
        impl->usStationValid = true;
    }

    bool Hpg::IsSlopeValid()
    {
        return impl->slopeValid;
    }

    double Hpg::GetSlope()
    {
        return impl->slope;
    }

    void Hpg::SetSlope(double slope)
    {
        impl->slope = slope;
        impl->slopeValid = true;
    }

    bool Hpg::IsLengthValid()
    {
        return impl->lengthValid;
    }

    double Hpg::GetLength()
    {
        return impl->length;
    }

    void Hpg::SetLength(double length)
    {
        impl->length = length;
        impl->lengthValid = true;
    }

    bool Hpg::IsRoughnessValid()
    {
        return impl->roughnessValid;
    }

    double Hpg::GetRoughness()
    {
        return impl->roughness;
    }

    void Hpg::SetRoughness(double roughness)
    {
        impl->roughness = roughness;
        impl->roughnessValid = true;
    }

    bool Hpg::IsMaxDepthValid()
    {
        return impl->maxDepthValid;
    }

    double Hpg::GetMaxDepth()
    {
        return impl->maxDepth;
    }

    void Hpg::SetMaxDepth(double maxDepth)
    {
        impl->maxDepth = maxDepth;
        impl->maxDepthValid = true;
    }

    bool Hpg::IsUnsteadyDepthPctValid()
    {
        return impl->unsteadyDepthPctValid;
    }

    double Hpg::GetUnsteadyDepthPct()
    {
        return impl->unsteadyDepthPct;
    }

    void Hpg::SetUnsteadyDepthPct(double maxdepth)
    {
        impl->unsteadyDepthPct = maxdepth;
        impl->unsteadyDepthPctValid = true;
    }


    bool  point_ax_lt_bx(const point& a, const point& b)
    {
        if (a.x < b.x)
            return true;
        else
            return false;
    }


    bool  point_ay_lt_by(const point& a, const point& b)
    {
        if (a.y < b.y)
            return true;
        else
            return false;
    }


    bool  point_av_lt_bv(const point& a, const point& b)
    {
        if (a.v < b.v)
            return true;
        else
            return false;
    }


    bool  point_ax_gt_bx(const point& a, const point& b)
    {
        if (a.x > b.x)
            return true;
        else
            return false;
    }


    bool  point_ay_gt_by(const point& a, const point& b)
    {
        if (a.y > b.y)
            return true;
        else
            return false;
    }


    bool  point_av_gt_bv(const point& a, const point& b)
    {
        if (a.v > b.v)
            return true;
        else
            return false;
    }
}
