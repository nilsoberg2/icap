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

#ifndef CROSS_SECTION_H__
#define CROSS_SECTION_H__

#include <string>
#include <vector>
#include <memory>

#include "../api.h"
#include "../util/parseable.h"

#include "types.h"


namespace xs
{
    class CrossSection : public Parseable
    {
    protected:
        xstype xsType;
    public:
        virtual ~CrossSection() { }
        virtual bool setParameters(std::vector<std::string>::const_iterator firstPart, std::vector<std::string>::const_iterator end) = 0;

        virtual xstype getType() const { return this->xsType; }
        virtual double getMaxDepth() = 0;

        virtual double computeArea(double depth) = 0;
        virtual double computeWettedPerimiter(double depth) = 0;
        virtual double computeTopWidth(double depth) = 0;
        virtual double computeDpDy(double depth) = 0;
        virtual double computeDaDy(double depth) = 0;
        virtual double computeDtDy(double depth) = 0;

        virtual std::shared_ptr<CrossSection> clone() = 0;
    };

    class Factory
    {
    public:
        static CrossSection* create(const CrossSection* xs);
        static CrossSection* create(xstype xsType);
        static CrossSection* create(const std::string& type);
        template<typename XSType>
        static CrossSection* create();
    };
}


#endif//CROSS_SECTION_H__
