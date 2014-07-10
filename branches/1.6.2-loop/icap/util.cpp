#define _CRT_SECURE_NO_DEPRECATE

#include "util.h"
#include "icap.h"
#include "icapnetwork.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include "timefuncs.h"


bool readCoordLine(char* line, ICAPNetwork& network);
bool readVertLine(char* line, ICAPNetwork& network);



extern "C"
{

	// From http://www.techbytes.ca/techbyte103.html --- Thanks!
int FileExists(const char* strFilename) {
    struct stat stFileInfo;
    bool blnReturn;
    int intStat;

    // Attempt to get the file attributes
    intStat = stat(strFilename,&stFileInfo);
    if(intStat == 0) {
        // We were able to get the file attributes
        // so the file obviously exists.
        blnReturn = true;
    } else {
        // We were not able to get the file attributes.
        // This may mean that we don't have permission to
        // access the folder which contains this file. If you
        // need to do that level of checking, lookup the
        // return values of stat which will give you
        // more details on why stat failed.
        blnReturn = false;
    }

    return(blnReturn);
}

}



void GetTempFileName(char* file)
{
    char temp[13];
    memset(temp, 0, sizeof(temp));
    int i = 0;

    for (i = 0; i < 8; i++)
        temp[i] = rand() % 26 + 97;
    temp[8] = '.';
    temp[9] = 't';
    temp[10] = 'm';
    temp[11] = 'p';
    temp[12] = '\0';

    i = 0;
    while (FileExists(temp))
    {
        temp[i] = rand() % 26 + 97;
        i++;
        if (i > 7)
            i = 0;
    }

    strcpy(file, ".\\");
    strcat(file, temp);
}


int TSDateTimeStrLen()
{
    // 8 for the time, 1 for the space, 11 for the date, and 1 for
    // the null terminating char.
    return 8 + 1 + 11 + 1;
}

void TSDateTimeStr(double currentDate, char* datetimeBuf)
{
    char timeBuf[9];
    char dateBuf[12];
    
    datetime_timeToStr(currentDate, timeBuf);
    datetime_dateToStr(currentDate, dateBuf);

    strcpy(datetimeBuf, dateBuf);
    strcat(datetimeBuf, " ");
    strcat(datetimeBuf, timeBuf);
}


bool GetSWMMCoordsAndVerts(const char* file, ICAPNetwork& network)
{
#if defined(SWMM_GEOMETRY)
    FILE* fh = fopen(file, "r");
    if (fh == NULL)
        return false;

    int coordLen = (int)strlen(ws_COORDINATE);
    int vertLen = (int)strlen(ws_VERTICES);
    int section = 0;

    char* line = new char[MAXLINE];
    memset(line, 0, sizeof(line));

    int validCount = 0;

    while (!feof(fh))
    {
        if (fgets(line, MAXLINE, fh) == NULL)
            break;

        if (!strncmp(line, ws_COORDINATE, coordLen))
        {
            section = s_COORDINATE;
            // found coordinate section
        }
        else if (!strncmp(line, ws_VERTICES, vertLen))
        {
            section = s_VERTICES;
            // found vertices section
        }
        else if (!strncmp(line, "[", 1))
        {
            section = 0;
        }
        else if (section == s_COORDINATE && strlen(line) > 2 && line[0] != ';')
        {
            if (readCoordLine(line, network))
                validCount++;
        }
        else if (section == s_VERTICES && strlen(line) > 2 && line[0] != ';')
        {
            if (readVertLine(line, network))
                validCount++;
        }
    }

    fclose(fh);

	delete[] line;

    if (validCount > 0)
        return true;
    else
        return false;
#else
    return true;
#endif
}



bool readCoordLine(char* line, ICAPNetwork& network)
{
#if defined(SWMM_GEOMETRY)
    char* parts = strtok(line, " ");
    int partCount = 0;
    
    int idx = -1;
    ICAPNode* node = NULL;
    double x, y;

    while (parts != NULL)
    {
        partCount++;
        if (partCount == 1) // read ID
        {
            idx = project_findObject(NODE, parts);
            node = network.FindNode(idx);
            if (node == NULL)
                return false;
        }
        else if (partCount == 2) // read x coord
            x = atof(parts);
        else if (partCount == 3) // read y coord
            y = atof(parts);
        parts = strtok(NULL, " ");
    }

    if (partCount < 3 || node == NULL)
        return false;

    for (int i = 0; i < (int)node->linkIdx.size(); i++)
    {
        int linkIdx = node->linkIdx.at(i);
        if (GLINK_USNODE(linkIdx) == idx)
        {
            ICAPLink* link = network.FindLink(linkIdx);
            if (link == NULL)
                link = network.MakeLink(linkIdx);

            link->x1 = x;
            link->y1 = y;
        }
        else if (GLINK_DSNODE(linkIdx) == idx)
        {
            ICAPLink* link = network.FindLink(linkIdx);
            if (link == NULL)
                link = network.MakeLink(linkIdx);

            link->x2 = x;
            link->y2 = y;
        }
    }
#endif

    return true;
}



bool readVertLine(char* line, ICAPNetwork& network)
{
#if defined(SWMM_GEOMETRY)
    char* parts = strtok(line, " ");
    int partCount = 0;
    
    int idx = -1;
    double x, y;
    ICAPLink* link = NULL;

    while (parts != NULL)
    {
        partCount++;
        if (partCount == 1) // read ID
        {
            idx = project_findObject(LINK, parts);
            link = network.FindLink(idx);
            if (link == NULL)
                link = network.MakeLink(idx);
        }
        else if (partCount == 2) // read x coord
            x = atof(parts);
        else if (partCount == 3) // read y coord
            y = atof(parts);
        parts = strtok(NULL, " ");
    }

    if (partCount < 3 || link == NULL)
    {
		//if (link != NULL)
		//{
		//	delete link;
		//}

        return false;
    }

    link->verts.push_back(ICAPPair(x, y));
#endif

    return true;
}
