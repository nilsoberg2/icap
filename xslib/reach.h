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

#ifndef REACH_H__
#define REACH_H__


#include <memory>

#include "cross_section.h"


namespace xs
{
    class Reach
    {
    private:
        std::shared_ptr<CrossSection> xs;
        double length;
        double dsInvert;
        double usInvert;
        double dsStation;
        double usStation;
        double roughness;

    public:
        Reach() : length(0), dsInvert(0), usInvert(0), dsStation(-99999), usStation(-99999), roughness(0) {}
        std::shared_ptr<CrossSection> getXs() const { return this->xs; }
        void setXs(std::shared_ptr<CrossSection> value) { this->xs = value; }

        const double getLength() const  { return this->length; }
		void setLength(double value) { this->length = value; }
        const double getDsInvert() const  { return this->dsInvert; }  
		void setDsInvert(double value) { this->dsInvert = value; }
        const double getUsInvert() const  { return this->usInvert; }  
		void setUsInvert(double value) { this->usInvert = value; }
        const double getDsStation() const  { return this->dsStation; }  
		void setDsStation(double value) { this->dsStation = value; }
        const double getUsStation() const  { return this->usStation; }  
		void setUsStation(double value) { this->usStation = value; }
        const double getRoughness() const  { return this->roughness; }  
        void setRoughness(double value) { this->roughness = value; }

        const double getMaxDepth() const  { return this->xs->getMaxDepth(); }  
        const double getSlope() const  { return (this->usInvert - this->dsInvert) / this->length; }  
    };
}


#endif//REACH_H__
