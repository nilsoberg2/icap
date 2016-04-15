// ==============================================================================
// ICAP License
// ==============================================================================
// University of Illinois/NCSA
// Open Source License
// 
// Copyright (c) 2014-2016 University of Illinois at Urbana-Champaign.
// All rights reserved.
// 
// Developed by:
// 
//     Nils Oberg
//     Blake J. Landry, PhD
//     Arthur R. Schmidt, PhD
//     Ven Te Chow Hydrosystems Lab
// 
//     University of Illinois at Urbana-Champaign
// 
//     https://vtchl.illinois.edu
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal with
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimers.
// 
//     * Redistributions in binary form must reproduce the above copyright notice,
//       this list of conditions and the following disclaimers in the
//       documentation and/or other materials provided with the distribution.
// 
//     * Neither the names of the Ven Te Chow Hydrosystems Lab, University of
// 	  Illinois at Urbana-Champaign, nor the names of its contributors may be
// 	  used to endorse or promote products derived from this Software without
// 	  specific prior written permission.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
// SOFTWARE.


#include "hpg.hpp"
#include "errors.hpp"
#include "impl.h"


namespace hpg
{
    Hpg::Hpg()
    {
        impl = new Impl();
        initialize();
    }

    Hpg::~Hpg()
    {
        deinitialize();
        delete impl;
    }

    //unsigned int Hpg::NumPosFlows()
    //{
    //    return impl->posFlowCount;
    //}

    //unsigned int Hpg::NumAdvFlows()
    //{
    //    return impl->advFlowCount;
    //}

    //double Hpg::PosFlowAt(unsigned int f)
    //{
    //    return impl->posFlows.at(f);
    //}

    //double Hpg::AdvFlowAt(unsigned int f)
    //{
    //    return impl->advFlows.at(f);
    //}

    //hpgvec& Hpg::PosValuesAt(unsigned int f)
    //{
    //    return impl->posValues.at(f);
    //}

    //hpgvec& Hpg::AdvValuesAt(unsigned int f)
    //{
    //    return impl->advValues.at(f);
    //}

    bool Hpg::isCurveSteep(unsigned int curve)
    {
        point p = impl->posValues.at(curve).at(0);
        if (p.x <= p.y)
            return false;
        else
            return true;
    }

    void Hpg::AddCurve(double flow, hpgvec& curve, point crit)
    {
        if (flow >= -1e-6)
        {
            impl->posFlowCount++;
            impl->posFlows.push_back(flow);
            impl->posValues.push_back(curve);
            impl->posCritical.push_back(crit);
            if (flow < impl->minPosFlow)
                impl->minPosFlow = flow;
            if (flow > impl->maxPosFlow)
                impl->maxPosFlow = flow;
        }
        else
        {
            impl->advFlowCount++;
            impl->advFlows.push_back(flow);
            impl->advValues.push_back(curve);
            impl->advCritical.push_back(crit);
            if (flow > impl->minAdvFlow)
                impl->minAdvFlow = flow;
            if (flow < impl->maxAdvFlow)
                impl->maxAdvFlow = flow;
        }
    }

    //hpgvec& Hpg::GetPosCritical()
    //{
    //    return impl->posCritical;
    //}

    //hpgvec& Hpg::GetAdvCritical()
    //{
    //    return impl->advCritical;
    //}

    void Hpg::deinitialize()
    {
        impl->SplPosCritUS_DS.clear();
        impl->SplPosCritUS_DS_ranges.clear();
    }

    void Hpg::initialize()
    {
        impl->errorCode = S_OK;
        impl->minPosFlow = 1000000.0;
        impl->maxPosFlow = 0.0;
        impl->maxAdvFlow = 0.0;
        impl->minAdvFlow = -1000000.0;
        //    minPosFlow = maxPosFlow = minAdvFlow = maxAdvFlow = 0.0;
        impl->posFlowCount = impl->advFlowCount = 0;

        impl->dsInvertValid = impl->usInvertValid = impl->dsStationValid = impl->usStationValid =
            impl->slopeValid = impl->lengthValid = impl->roughnessValid = impl->maxDepthValid = impl->unsteadyDepthPctValid = false;
        impl->dsInvert = impl->usInvert = impl->dsStation = impl->usStation = impl->slope =
            impl->length = impl->roughness = impl->maxDepth = impl->unsteadyDepthPct = 0.0;
        impl->nodeId = "";
        impl->version = 2;
    }

    void Hpg::copyFrom(const Hpg& copy)
    {
        impl->copyFrom(copy.impl);

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

        if (impl->posFlowCount || impl->advFlowCount)
            setupSplines();
    }

    int Hpg::getVersion()
    {
        return impl->version;
    }

    bool Hpg::isDsInvertValid()
    {
        return impl->dsInvertValid;
    }

    double Hpg::getDsInvert()
    {
        return impl->dsInvert;
    }

    void Hpg::setDsInvert(double dsinvert)
    {
        impl->dsInvert = dsinvert;
        impl->dsInvertValid = true;
    }

    std::string Hpg::getNodeId()
    {
        return impl->nodeId;
    }

    void Hpg::setNodeId(std::string id)
    {
        impl->nodeId = id;
    }

    bool Hpg::isUsInvertValid()
    {
        return impl->usInvertValid;
    }

    double Hpg::getUsInvert()
    {
        return impl->usInvert;
    }

    void Hpg::setUsInvert(double usinvert)
    {
        impl->usInvert = usinvert;
        impl->usInvertValid = true;
    }

    bool Hpg::isDsStationValid()
    {
        return impl->dsStationValid;
    }

    double Hpg::getDsStation()
    {
        return impl->dsStation;
    }

    void Hpg::setDsStation(double dsstation)
    {
        impl->dsStation = dsstation;
        impl->dsStationValid = true;
    }

    bool Hpg::isUsStationValid()
    {
        return impl->usStationValid;
    }

    double Hpg::getUsStation()
    {
        return impl->usStation;
    }

    void Hpg::setUsStation(double usstation)
    {
        impl->usStation = usstation;
        impl->usStationValid = true;
    }

    bool Hpg::isLengthValid()
    {
        return impl->lengthValid;
    }

    double Hpg::getLength()
    {
        return impl->length;
    }

    void Hpg::setLength(double length)
    {
        impl->length = length;
        impl->lengthValid = true;
    }

    bool Hpg::isRoughnessValid()
    {
        return impl->roughnessValid;
    }

    double Hpg::getRoughness()
    {
        return impl->roughness;
    }

    void Hpg::setRoughness(double roughness)
    {
        impl->roughness = roughness;
        impl->roughnessValid = true;
    }

    bool Hpg::isMaxDepthValid()
    {
        return impl->maxDepthValid;
    }

    double Hpg::getMaxDepth()
    {
        return impl->maxDepth;
    }

    void Hpg::setMaxDepth(double maxDepth)
    {
        impl->maxDepth = maxDepth;
        impl->maxDepthValid = true;
    }

    bool Hpg::isMaxDepthFractionValid()
    {
        return impl->unsteadyDepthPctValid;
    }

    double Hpg::getMaxDepthFraction()
    {
        return impl->unsteadyDepthPct;
    }

    void Hpg::setMaxDepthFraction(double maxdepth)
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
