
#define _CRT_SECURE_NO_DEPRECATE


#include "icap.h"
#include "icap_interface.h"
#include <cmath>
#include <iostream>
#include <ctime>



bool getCommandLineParams(int argc, char** argv, char* inputFile, char* reportFile, char* outputFile, char* tvcFile, bool& hasTVC, bool& useMatrix, float& resDepth, float& flowFactor, char* usNodeId, float& targetValue);
bool getConsoleParams(char* inputFile, char* reportFile, char* outputFile);



#if !defined(ICAP_EXPORT_DLL) && !defined(ICAP_IMPORT_DLL)

int main(int argc, char** argv)
{
    using namespace std;
    srand( (unsigned int)time(NULL) );

    // Get the files from the command line or console input.
    char inputFile[MAXFNAME + 1];
    char outputFile[MAXFNAME + 1];
    char reportFile[MAXFNAME + 1];
	char tvcFile[MAXFNAME + 1];
    char usNodeId[255];
    memset(usNodeId, 0, 255);

    float resDepth = -1;
    float flowFactor = 1.0;
    float targetHead = -1000000;
	bool hasTVCFile = false;
	bool useMatrix = false;
    if (! getCommandLineParams(argc, argv, inputFile, reportFile, outputFile, tvcFile, hasTVCFile, useMatrix, resDepth, flowFactor, usNodeId, targetHead))
    {
        if (! getConsoleParams(inputFile, reportFile, outputFile))
            DEBUG_EXIT;
    }

	printf("Opening ICAP...\n");
    ICAP icap;
    bool result = icap.Open(inputFile, reportFile, outputFile, true);
    if (! result)
        DEBUG_EXIT;
    
    // Set before Start()
    if (resDepth >= 0)
    {
        icap.SetReservoirDepth(resDepth);
    }

	printf("Starting ICAP...\n");
    result = icap.Start(useMatrix);
    if (! result)
        DEBUG_EXIT;

	if (hasTVCFile)
	{
		printf("Generating TVC...\n");
		icap.SaveTotalVolumeCurve(tvcFile);
		printf("Done.\n");
		return 0;
	}

    if (targetHead > -999999 && strlen(usNodeId) > 0)
    {
        int usNodeIdx = icap.FindNodeIndex(usNodeId);
        if (usNodeIdx < 0)
        {
            printf("Unable to find node %s, aborting\n", usNodeId);
        }
        else
        {
            int dsNodeIdx = icap.GetReservoirNodeIndex();
            float usNodeHead = icap.GetNodeInvert(dsNodeIdx);
            float min = 0;
            float max = 2;
            float flowFactor = 0.5;
            float dsFlow = 0;
            float dsNodeHead = 0;
            int numIter = 0;
            while (numIter++ < 50 && fabs(usNodeHead - targetHead) > 0.01)
            {
                icap.SetFlowFactor(flowFactor);
                icap.InitializeZeroDepths();
                icap.SetReservoirDepth(resDepth);
                double curStep = 0;
                if (!icap.Step(&curStep, false))
                {
                    printf("ERROR in step %s, aborting\n", icap.getErrorMessage());
                    break;
                }
                else
                {
                    usNodeHead = icap.GetNodeWaterElev(usNodeIdx);
                    dsNodeHead = icap.GetNodeWaterElev(dsNodeIdx);
                    dsFlow = icap.GetNodeFlow(dsNodeIdx);
                    printf("ITER\t%f\t%f\t%f\t%f\n", flowFactor, dsNodeHead, dsFlow, usNodeHead);
                    if (usNodeHead < targetHead)
                    {
                        min = flowFactor;
                        flowFactor = min + (max - min) / 2.0;
                    }
                    else if (usNodeHead > targetHead)
                    {
                        max = flowFactor;
                        flowFactor = min + (max - min) / 2.0;
                    }
                }
            }
            printf("FINAL\t%f\t%f\t%f\t%f\n", flowFactor, dsNodeHead, dsFlow, usNodeHead);
        }
    }
    else if (useMatrix)
	{
        double curStep = 0.0;

	    printf("Matrix...\n");
        // Now iterate over each timestep and route the system for that step.
        bool toContinue = true;
        int iterCount = 0;
        do
        {
		    printf("Iteration %f of %f...\n", curStep/1000.0, m_totalDuration/1000.0);
            result = icap.Step(&curStep, true);
            if (! result)
                break;
        } while (curStep <= m_totalDuration && !isZero(curStep));
	}
	else
    {
        double curStep = 0.0;

	    printf("Iterating...\n");
        // Now iterate over each timestep and route the system for that step.
        bool toContinue = true;
        int iterCount = 0;
        do
        {
		    printf("Iteration %f of %f...\n", curStep/1000.0, m_totalDuration/1000.0);
            result = icap.Step(&curStep, false);
            if (! result)
                break;
        } while (curStep <= m_totalDuration && !isZero(curStep));

    }

	printf("Wrapping up...\n");
    icap.End();

    icap.Close();

    return 0;
}

#endif




bool getCommandLineParams(int argc, char** argv, char* inputFile, char* reportFile, char* outputFile, char* tvcFile, bool& hasTVC, bool& useMatrix, float& resDepth, float& flowFactor, char* usNodeId, float& targetValue)
{
    if (argc < 7)
        return false;

    bool hasInput = false,
         hasReport = false,
         hasOutput = false;

	useMatrix = false;

    for (int i = 1; i < argc; i++)
    {
        if (! strcmp(argv[i], "-i") && i + 1 < argc)
        {
            i++;
            strcpy(inputFile, argv[i]);
            hasInput = true;
        }
        else if (! strcmp(argv[i], "-r") && i + 1 < argc)
        {
            i++;
            strcpy(reportFile, argv[i]);
            hasReport = true;
        }
        else if (! strcmp(argv[i], "-o") && i + 1 < argc)
        {
            i++;
            strcpy(outputFile, argv[i]);
            hasOutput = true;
        }
		else if (! strcmp(argv[i], "-tvc") && i + 1 < argc)
		{
			i++;
			strcpy(tvcFile, argv[i]);
			hasTVC = true;
		}
        else if (! strcmp(argv[i], "-rd") && i + 1 < argc)
        {
            i++;
            resDepth = atof(argv[i]);
        }
        else if (! strcmp(argv[i], "-ff") && i + 1 < argc)
        {
            i++;
            flowFactor = atof(argv[i]);
        }
        else if (! strcmp(argv[i], "-usid") && i + 1 < argc)
        {
            i++;
            strcpy(usNodeId, argv[i]);
        }
        else if (! strcmp(argv[i], "-ushead") && i + 1 < argc)
        {
            i++;
            targetValue = atof(argv[i]);
        }
		else if (! strcmp(argv[i], "-matrix"))
		{
			useMatrix = true;
		}
    }

    return (hasInput && hasReport && hasOutput);
}


bool getConsoleParams(char* inputFile, char* reportFile, char* outputFile)
{
    using namespace std;

    std::string input, report, output;

    int count = 0;


    // Get the input file.
    while (input.length() == 0 && count < 3)
    {
        cout << "Enter the path to an input file:" << endl;
        getline(cin, input);
        count++;
    }

    if (count == 3)
    {
        cout << "Failed to get input file.  Exiting." << endl;
        return false;
    }

    strcpy(inputFile, input.c_str());


    // Get the report file.
    count = 0;
    while (report.length() == 0 && count < 3)
    {
        cout << "Enter the path to a report file:" << endl;
        getline(cin, report);
        count++;
    }

    if (count == 3)
    {
        cout << "Failed to get report file.  Exiting." << endl;
        return false;
    }

    strcpy(reportFile, report.c_str());


    // Get the output file.
    count = 0;
    while (output.length() == 0 && count < 3)
    {
        cout << "Enter the path to an output file:" << endl;
        getline(cin, output);
        count++;
    }

    if (count == 3)
    {
        cout << "Failed to get output file.  Exiting." << endl;
        return false;
    }

    strcpy(outputFile, output.c_str());

    return true;
}
