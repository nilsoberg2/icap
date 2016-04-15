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

#include "stdafx.h"
#include "CppUnitTest.h"
#include <Windows.h>
#include "../geometry/geometry.h"
#include "../geometry/storage.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>
#include "../xslib/reach.h"
#include "../hpg_creation/hpg_creator.hpp"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace geometry;
namespace fs = boost::filesystem;

namespace TestGeometryRead
{		
	TEST_CLASS(GeometryTest)
	{
	public:

        static bool idComp(const std::string& id1, const std::string& id2)
        {
            using namespace std;
            cout << id1 << " " << id2 << endl;
            return id1 == id2;
        }

        std::shared_ptr<Geometry> loadGeometry(bool& status)
        {
            wchar_t cwd[MAX_PATH];
            _wgetcwd(cwd, MAX_PATH);
            std::wstring temp(cwd);
            fs::path filePath(temp.begin(), temp.end());
            filePath = filePath / ".." / "geometry_test.inp";
            
            std::shared_ptr<Geometry> g = std::shared_ptr<Geometry>(new Geometry());
            status = g->loadFromFile(filePath.string(), geometry::GeometryFileFormat::FileFormatSwmm5);
            return g;
        }

		TEST_METHOD(ParseGeometry)
		{
            using namespace std;
            bool status;
			std::shared_ptr<Geometry> g = loadGeometry(status);
            Assert::IsTrue(status, makeInfo(L"Failed to load geometry file: ", g->getErrorMessage()).c_str());

            const vector<string>& linkIds = g->getLinkIds();
            const vector<string>& nodeIds = g->getNodeIds();

            const vector<string>& actualLinkIds = getLinkIds();
            const vector<string>& actualNodeIds = getNodeIds();

            //std::pair<std::vector<std::string>::iterator, std::vector<std::string>::iterator> endLinkPair = std::make_pair(linkIds.end(), actualLinkIds.end());
            //std::pair<std::vector<std::string>::iterator, std::vector<std::string>::iterator> endNodePair = std::make_pair(nodeIds.end(), actualNodeIds.end());
            //Assert::IsTrue(std::equal(linkIds.begin(), linkIds.end(), actualLinkIds.begin(), idComp), L"Link list is not equal");
            //Assert::IsTrue(std::equal(nodeIds.begin(), nodeIds.end(), actualNodeIds.begin(), idComp), L"Node list is not equal");
            Assert::IsTrue(vectorEqual<string>(linkIds, actualLinkIds), L"Link list is not equal");
            Assert::IsTrue(vectorEqual<string>(nodeIds, actualNodeIds), L"Node list is not equal");
            
        }

        TEST_METHOD(Create40_1HpgTest)
        {
            using namespace std;
            bool status;
            std::shared_ptr<Geometry> g = loadGeometry(status);
            Assert::IsTrue(status, makeInfo(L"Failed to load geometry file: ", g->getErrorMessage()).c_str());

            wchar_t cwd[MAX_PATH];
            _wgetcwd(cwd, MAX_PATH);
            std::wstring temp(cwd);
            fs::path dirPath(temp.begin(), temp.end());
            dirPath = dirPath / ".." / "hpgs";
            fs::create_directory(dirPath);

            const vector<string>& linkIds = g->getLinkIds();
            const vector<string>& nodeIds = g->getNodeIds();

            string id = "40-1";
            //for (auto id : linkIds)
            //{
                auto link = g->getLink(id);
                xs::Reach reach;
                reach.setDsInvert(link->getDownstreamInvert());
                reach.setUsInvert(link->getUpstreamInvert());
                reach.setLength(link->getLength());
                reach.setRoughness(link->getRoughness());
                reach.setXs(link->getXs());

                HpgCreator hpgC;
                auto hpg = hpgC.AutoCreateHpg(reach);
                string filePath = (dirPath / (id + ".txt")).string();
                hpg->SaveToFile(filePath);
            //}
        }

		TEST_METHOD(CreateHpgTest)
		{
            using namespace std;
            bool status;
			std::shared_ptr<Geometry> g = loadGeometry(status);
            Assert::IsTrue(status, makeInfo(L"Failed to load geometry file: ", g->getErrorMessage()).c_str());

            wchar_t cwd[MAX_PATH];
            _wgetcwd(cwd, MAX_PATH);
            std::wstring temp(cwd);
            fs::path dirPath(temp.begin(), temp.end());
            dirPath = dirPath / ".." / "hpgs";
            fs::create_directory(dirPath);

            const vector<string>& linkIds = g->getLinkIds();
            const vector<string>& nodeIds = g->getNodeIds();

            for (auto id: linkIds)
            {
                auto link = g->getLink(id);
                xs::Reach reach;
                reach.setDsInvert(link->getDownstreamInvert());
                reach.setUsInvert(link->getUpstreamInvert());
                reach.setLength(link->getLength());
                reach.setRoughness(link->getRoughness());
                reach.setXs(link->getXs());

                HpgCreator hpgC;
                auto hpg = hpgC.AutoCreateHpg(reach);
                string filePath = (dirPath / (id + ".txt")).string();
                hpg->SaveToFile(filePath);
            }
		}

		TEST_METHOD(CurveTest)
		{
            using namespace std;
            bool status;
			std::shared_ptr<Geometry> g = loadGeometry(status);
            Assert::IsTrue(status, makeInfo(L"Failed to load geometry file: ", g->getErrorMessage()).c_str());

            std::shared_ptr<Node> node = g->getNode("outlet");
            std::shared_ptr<StorageUnit> storNode;
            if ((storNode = dynamic_pointer_cast<StorageUnit>(node)) == NULL)
            {
                Assert::Fail(L"Expected StorageUnit; instead got a Node");
            }

            const std::shared_ptr<Curve> c = storNode->getStorageCurve();
            
            Assert::AreEqual(0.0, c->lookup(-1));
            Assert::AreEqual(0.0, c->lookup(0));
            Assert::AreEqual(10.0, c->lookup(5));
            Assert::AreEqual(20.0, c->lookup(10));
            Assert::AreEqual(20.0, c->lookup(12));
		}

		TEST_METHOD(OptionsTest)
		{
            using namespace std;
            bool status;
			std::shared_ptr<Geometry> g = loadGeometry(status);
            Assert::IsTrue(status, makeInfo(L"Failed to load geometry file: ", g->getErrorMessage()).c_str());

            Assert::IsTrue(g->hasOption("hpg_path"));
            Assert::AreEqual(g->getOption("hpg_path"), std::string("lawrence hpgs"));
            
            Assert::IsTrue(g->hasOption("dummy"));
            Assert::AreEqual(g->getOption("dummy"), std::string("lawrence hpgs"));

            Assert::IsTrue(g->hasOption("routing_step"));
            Assert::AreEqual(g->getOption("routing_step"), std::string("300"));

            DateTime dt(2010, 7, 23, 1, 0, 0);
            Assert::AreEqual(g->getStartDateTime(), dt);
		}

		TEST_METHOD(TimeseriesTest)
		{
            using namespace std;
            bool status;
			std::shared_ptr<Geometry> g = loadGeometry(status);
            Assert::IsTrue(status, makeInfo(L"Failed to load geometry file: ", g->getErrorMessage()).c_str());

            DateTime start = g->getStartDateTime();

            std::shared_ptr<Node> node = g->getNode("laramie");
            std::shared_ptr<Inflow> inf = node->getInflow();

            Assert::IsTrue(inf != NULL);
            
            Assert::AreEqual(0.0, inf->getInflow(start.addHours(18)));
            Assert::AreEqual(1.0, inf->getInflow(start.addHours(18).addMinutes(12)));
            Assert::AreEqual(5.0, inf->getInflow(start.addHours(18).addMinutes(16)));
            Assert::AreEqual(4.5, inf->getInflow(start.addHours(18).addMinutes(16).addSeconds(30)));
            Assert::AreEqual(0.0, inf->getInflow(start.addHours(21)));
		}

        template<class T>
        bool vectorEqual(const std::vector<T>& v1, const std::vector<T>& v2) const
        {
            if (v1.size() != v2.size())
            {
                return false;
            }

            for (int i = 0; i < v1.size(); i++)
            {
                if (v1[i] != v2[i])
                    return false;
            }

            return true;
        }

        std::vector<std::string> getLinkIds() const
        {
            std::vector<std::string> ids;
            ids.push_back("6");
            ids.push_back("11");
            ids.push_back("18");
            ids.push_back("37");
            ids.push_back("36-1");
            ids.push_back("36-2");
            ids.push_back("36-3");
            ids.push_back("36-4");
            ids.push_back("35-1");
            ids.push_back("35-2");
            ids.push_back("34-1");
            ids.push_back("34-2");
            ids.push_back("40-1");
            ids.push_back("40-2");
            ids.push_back("40-3");
            ids.push_back("40-4");
            ids.push_back("38-1");
            ids.push_back("38-2");
            ids.push_back("39-1");
            ids.push_back("39-2");
            ids.push_back("39-3");
            ids.push_back("39-4");
            std::sort(ids.begin(), ids.end());
            return ids;
        }

        std::vector<std::string> getNodeIds() const
        {
            std::vector<std::string> ids;
            ids.push_back("outlet");
            ids.push_back("melvina");
            ids.push_back("menard");
            ids.push_back("long");
            ids.push_back("laramie");
            ids.push_back("kilbourn");
            ids.push_back("kildare");
            ids.push_back("harding");
            ids.push_back("berteau");
            ids.push_back("montrose");
            ids.push_back("drake");
            ids.push_back("t36-1");
            ids.push_back("t36-2");
            ids.push_back("t36-3");
            ids.push_back("t35-1");
            ids.push_back("t34-1");
            ids.push_back("t40-1");
            ids.push_back("t40-2");
            ids.push_back("t40-3");
            ids.push_back("t38-1");
            ids.push_back("t39-1");
            ids.push_back("t39-2");
            ids.push_back("t39-3");
            std::sort(ids.begin(), ids.end());
            return ids;
        }

        std::wstring makeInfo(wchar_t* p1, std::string p2)
        {
            std::wstring str = p1;
            str = str + std::wstring(p2.begin(), p2.end());
            return str;
        }

	};

}

namespace Microsoft{
    namespace VisualStudio {
        namespace CppUnitTestFramework {

            template<>
            static std::wstring ToString<DateTime>(const DateTime& dt) {
                return dt.toUnicodeString();
            }

        }
    }
}
