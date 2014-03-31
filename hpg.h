#ifndef __HPG_LIST_H________________________20080814110707__
#define __HPG_LIST_H________________________20080814110707__

#include <circhpg.hpp>
#include <vector>
#include <string>


#define HPG_ERROR -100
#define HPG_PIPE_FULL 1
#define HPG_PIPE_OK 0
#define HPG_PIPE_EMPTY -1


class ICAPHPG
{
protected:
    std::vector<hpgns::CircularHPG*> m_list;
    
    int m_hpgCount;

    int m_errorCode;
    std::string m_errorStr;

    NormCritParams m_ncParams;
    bool m_ncParamsInit;

    bool checkAndLoadHPG(int idx, const char* dir);

    bool loadHPG(int idx, const char* path);

    void setError(int code, std::string error);

    int m_currentHPG;

public:
    ~ICAPHPG();
    ICAPHPG();
    bool Allocate(int count);
    void Free();

    hpgns::CircularHPG* GetHPG(int idx);

    bool IsValidFlow(int linkIdx, double flow);

    bool CanInterpolate(int linkIdx, double dsDepth, double flow);

	// 0 = ok, -1 = too small flow, +1 = too large flow
	int CanInterpolateExtended(int linkIdx, double dsDepth, double flow);
    
    bool GetUpstream(int linkIdx, double dsDepth, double flow, double& usDepth);

	bool GetVolume(int linkIdx, double dsDepth, double flow, double& volume);
    
    bool GetCritUpstream(int linkIdx, double flow, double& usDepth);
    
    bool PressurizedInterpolation(int linkIdx, double dsDepth, double flow, double& usDepth);

    bool CalculateCriticalDepth(double flow, double diameter, double initGuess, double& hCrit);

    bool LoadHPGs(char* path);

    int LoadNextHPG(char* path);

	double GetLowestFlow(int linkIdx, double inputFlow);

    const char* GetErrorStr();
    int GetErrorCode();
    int GetHPGError(int linkIdx);
};


#endif//__HPG_LIST_H________________________20080814110707__
