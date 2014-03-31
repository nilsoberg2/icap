#ifndef __ICAP_INIT_H_______________________20080806160202__
#define __ICAP_INIT_H_______________________20080806160202__
#include "icap.h"
#include "error.h"
#include "source_list.h"
#include "network.h"
#include "icapnetwork.h"


bool runSimulation(SourceList& sources, ICAPNetwork& network, int sinkNodeIdx);
bool loadInputFile(char* inputFile, char* reportFile, char* outputFile);
bool loadOverflowHeight();
bool populateNetwork(char* inputFile, ICAPNetwork& network, int& sinkType, int& sinkNodeIdx, int& sinkLinkIdx);
bool loadHPGs(char* hpgPath);
bool findInputs(SourceList& sourceList);
bool openOutputFile(char* outputFile);
void cleanupExit();
void getTempFileName(char* file);


#endif//__ICAP_INIT_H_______________________20080806160202__
