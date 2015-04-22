
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>

#include "errors.hpp"
#include "split.hpp"
#include "hpg.hpp"
#include "impl.h"


namespace hpg
{
    typedef std::vector<std::string> svec;

    bool Hpg::LoadFromFile(const std::string& file, bool splineSetup)
    {
        using namespace std;

        impl->errorCode = S_OK;
        int status = S_OK;

        ifstream fh(file);
        if (! fh.is_open())
        {
            impl->errorCode = err::FileReadFailed;
            return false;
        }
        else if (fh.eof())
            return false;

        //
        // Parse up the header line and obtain attributes.
        //
        this->loadHeader(fh);

        // If there isn't anything else to read, this file is empty and
        // we return true (true because the HPG is valid, it's just empty).
        if (fh.eof())
            return true; 

        // Read the file. We're looking for lines that start with 'Q=' to
        // signal a new curve.
        string comp("Q=");

        string line = "";
        getline(fh, line);

        // temporary storage
        hpgvec values;
        double curFlow;
        int lineCount = 1;

        // Iterate through the file
        while (! fh.eof())
        {
            // Get the next line
            getline(fh, line);
            lineCount++;

            // Got a Q=... line
            if (line.substr(0, 2) == comp)
            {
                // We want to exit if the header isn't valid (i.e. it doesn't have a
                // discharge value).  Maybe the file is corrupt and we don't want to
                // read it.
                if (line.length() < 3)
                {
                    impl->errorCode = err::InvalidFileFormat;
                    return false;
                }
                else
                {
                    // Only add to the values list if this isn't the first time we've
                    // encountered a discharge header.  Values are added to the values
                    // list at the end of the discharge block.
                    if (lineCount > 2)
                    {
                        AddCurve(curFlow, values, values.front());
                        // Erase so we can start over
                        values.clear();
                    }

                    // Extract the actual discharge and add it to the discharges array.
                    curFlow = atof(line.substr(2).c_str());
                }
            }
            else if (line.length())
            {
                // Extract the parts of the value line
                svec parts;
                wiess_split(line.c_str(), "\t", parts, true);

                // We want to exit if there aren't at least 2 parts to the line.  This means
                // that the line isn't valid, indicating a corrupt file.
                if (parts.size() < 2)
                {
                    impl->errorCode = err::InvalidFileFormat;
                    return false;
                }

                // If there are only two parts (downstream, upstream)
                else if (parts.size() < 3)
                    values.push_back(point(atof(parts.at(0).c_str()), atof(parts.at(1).c_str())));

                // There should be three parts (downstream, upstream, volume) to the line
                else if (parts.size() < 4)
                    values.push_back(point(atof(parts.at(0).c_str()), atof(parts.at(1).c_str()), atof(parts.at(2).c_str())));

                // There should be four parts (downstream, upstream, volume, hf) to the line
                else
                    values.push_back(point(atof(parts.at(0).c_str()), atof(parts.at(1).c_str()), atof(parts.at(2).c_str()), atof(parts.at(3).c_str())));
            }
        }

        // Add the last read curve (doesn't get added in the previous
        // loop, since the last line will never be a 'Q=' line.
        if (values.size())
            AddCurve(curFlow, values, values.front());
        values.clear();

        fh.close();

        //PostLoadActions();

        if (splineSetup)
        {
            if (HPGFAILURE(status = setupSplines()))
            {
                //errorCode = status;
                return false;
            }
        }

        return true;
    }

    bool Hpg::loadHeader(std::ifstream& fh)
    {
        using namespace std;

        // Get the header line.
        string line;
        getline(fh, line);
        svec parts;
        wiess_split(line.c_str(), " ", parts, true);

        if (parts.at(0) != "HPG")
        {
            impl->errorCode = err::InvalidFileFormat;
            fh.close();
            return false;
        }

        // Load the attributes from the header line.
        for (unsigned int p = 1; p < parts.size(); p++)
        {
            svec kv;
            wiess_split(parts.at(p).c_str(), "=", kv, true);
            if (kv.size() > 1)
            {
                if (kv.at(0) == "nid")
                {
                    impl->nodeId = kv.at(1);
                }
                else if (kv.at(0) == "ver")
                {
                    impl->version = atoi(kv.at(1).c_str());
                }
                else if (kv.at(0) == "ds_invert")
                {
                    impl->dsInvert = atof(kv.at(1).c_str());
                    impl->dsInvertValid = true;
                }
                else if (kv.at(0) == "us_invert")
                {
                    impl->usInvert = atof(kv.at(1).c_str());
                    impl->usInvertValid = true;
                }
                else if (kv.at(0) == "dsta" || kv.at(0) == "ds_sta")
                {
                    impl->dsStation = atof(kv.at(1).c_str());
                    impl->dsStationValid = true;
                }
                else if (kv.at(0) == "usta" || kv.at(0) == "us_sta")
                {
                    impl->usStation = atof(kv.at(1).c_str());
                    impl->usStationValid = true;
                }
                //else if (kv.at(0) == "slope")
                //{
                //    impl->slope = atof(kv.at(1).c_str());
                //    impl->slopeValid = true;
                //}
                else if (kv.at(0) == "diameter" || kv.at(0) == "max_chan_depth")
                {
                    impl->maxDepth = atof(kv.at(1).c_str());
                    impl->maxDepthValid = true;
                }
                else if (kv.at(0) == "length")
                {
                    impl->length = atof(kv.at(1).c_str());
                    impl->lengthValid = true;
                }
                else if (kv.at(0) == "roughness")
                {
                    impl->roughness = atof(kv.at(1).c_str());
                    impl->roughnessValid = true;
                }
                else if (kv.at(0) == "maxdepth" || kv.at(0) == "unsteadydepth" || kv.at(0) == "max_depth_frac")
                {
                    impl->unsteadyDepthPct = atof(kv.at(1).c_str());
                    impl->unsteadyDepthPctValid = true;
                }
            }
        }

        if (impl->dsInvertValid && impl->usInvertValid && impl->lengthValid)
        {
            impl->slope = (impl->usInvert - impl->dsInvert) / impl->length;
            impl->slopeValid = true;
        }

        return true;
    }

    bool Hpg::SaveToFile(const std::string& file, bool append)
    {
        impl->errorCode = S_OK;

        // We're using <stdio.h> stuff (i.e. FILE* and fprintf) since we
        // have greater control over formatting of numbers.

        // Open the file for writing.  Return failed (false) if the
        // mode (action) is other than append or overwrite.
        // If the append flag is set, then we append instead of creating the file.
        FILE *fh = NULL;
        if (append)
            fh = fopen(file.c_str(), "a");
        else
            fh = fopen(file.c_str(), "w");

        if (fh == NULL)
        {
            impl->errorCode = err::FileWriteFailed;
            return false;
        }

        // Format the header line and only print it out if we're not appending.
        if (! append)
        {
            std::string header = saveHeader();
            // Print the header lines
            fprintf(fh, "#HPG %s\n", header.c_str());
            fprintf(fh, "#Downstream depth\tUpstream depth\tWater volume\n");
        }

        // Print out the curves to the file
        for (unsigned int i = 0; i < impl->posFlows.size(); i++)
        {
            fprintf(fh, "Q=%.1f\n", impl->posFlows[i]);

            hpgvec vec = impl->posValues[i];
            for (unsigned int j = 0; j < vec.size(); j++)
			{
				if (vec[j].hf_valid)
				{
	                fprintf(fh, "%.6f\t%.6f\t%.6f\t%.12f\n", vec[j].x, vec[j].y, vec[j].v, vec[j].hf);
				}
				else 
				{
					fprintf(fh, "%.6f\t%.6f\t%.6f\n", vec[j].x, vec[j].y, vec[j].v);
				}
			}

            fprintf(fh, "\n\n");
		}
        for (unsigned int i = 0; i < impl->advFlows.size(); i++)
        {
            fprintf(fh, "Q=%.1f\n", impl->advFlows[i]);

            hpgvec vec = impl->advValues[i];
            for (unsigned int j = 0; j < vec.size(); j++)
			{
				if (vec[j].hf_valid)
				{
	                fprintf(fh, "%.6f\t%.6f\t%.6f\t%.12f\n", vec[j].x, vec[j].y, vec[j].v, vec[j].hf);
				}
				else 
				{
					fprintf(fh, "%.6f\t%.6f\t%.6f\n", vec[j].x, vec[j].y, vec[j].v);
				}
			}

            fprintf(fh, "\n\n");
        }

        //
        // Flush and close the filehandle.
        //
        fflush(fh);
        fclose(fh);

        return true;
    }

    std::string Hpg::saveHeader()
    {
        std::stringstream header;

        if (impl->version != -1)
            header << "ver=" << impl->version << " ";

        header << "nid=" << impl->nodeId << " ";

        if (impl->dsInvertValid)
            header << "ds_invert=" << impl->dsInvert << " ";
        else
            header << "ds_invert= ";

        if (impl->usInvertValid)
            header << "us_invert=" << impl->usInvert << " ";
        else
            header << "us_invert= ";

        if (impl->dsStationValid)
            header << "ds_sta=" << impl->dsStation << " ";
        else
            header << "ds_sta= ";

        if (impl->usStationValid)
            header << "us_sta=" << impl->usStation << " ";
        else
            header << "us_sta= ";

        //if (impl->slopeValid)
        //    header << "slope=" << impl->slope << " ";
        //else
        //    header << "slope= ";

        if (impl->lengthValid)
            header << "length=" << impl->length << " ";
        else
            header << "length= ";

        if (impl->roughnessValid)
            header << "roughness=" << impl->roughness << " ";
        else
            header << "roughness= ";

        if (impl->maxDepthValid)
            header << "max_chan_depth=" << impl->maxDepth << " ";
        else
            header << "max_chan_depth= ";

        if (impl->unsteadyDepthPctValid)
            header << "max_depth_frac=" << impl->unsteadyDepthPct << " ";
        else
            header << "max_depth_frac= ";

        return header.str();
    }
}

