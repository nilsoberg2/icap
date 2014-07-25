#ifndef __HPG_LIST_H________________________20080814110707__
#define __HPG_LIST_H________________________20080814110707__

#include <vector>
#include <string>
#include <memory>

#include "../hpg_interp/hpg.hpp"
#include "../util/parseable.h"
#include "../geometry/link_list.h"


#define HPG_ERROR -100
#define HPG_PIPE_FULL 1
#define HPG_PIPE_OK 0
#define HPG_PIPE_EMPTY -1


class IcapHpg : public Parseable
{
protected:
    std::map<id_type, std::shared_ptr<hpgns::BaseHPG>> m_list;
    
    int m_hpgCount;

    //NormCritParams m_ncParams;
    //bool m_ncParamsInit;

    bool checkAndLoadHPG(std::shared_ptr<geometry::Link> link, const std::string& dirPath);

    bool loadHPG(id_type linkId, const std::string& path);

    int m_currentHPG;

public:
    ~IcapHpg();
    IcapHpg();
    bool allocate(int count);

    std::shared_ptr<hpgns::BaseHPG> getHpg(id_type idx);
    bool getUpstream(id_type linkId, var_type dsHead, var_type flow, var_type& usHead);
    bool getHf(id_type linkId, var_type dsHead, var_type flow, var_type& hf);
	bool getVolume(id_type linkId, var_type dsHead, var_type flow, var_type& volume);
    var_type getLowestFlow(int linkId, bool isAdverse);
    
    bool loadHpgs(const std::string& path, std::shared_ptr<geometry::LinkList> linkList);
    int loadNextHpg(const std::string& path, std::shared_ptr<geometry::LinkList> linkList);

    //bool IsValidFlow(int linkId, double flow);
    //bool CanInterpolate(int linkId, double dsDepth, double flow);
    //// 0 = ok, -1 = too small flow, +1 = too large flow
    //int CanInterpolateExtended(int linkId, double dsDepth, double flow);
    //bool GetCritUpstream(int linkId, double flow, double& usDepth);
    //bool PressurizedInterpolation(int linkId, double dsDepth, double flow, double& usDepth);
    //bool CalculateCriticalDepth(double flow, double diameter, double initGuess, double& hCrit);
    //int GetHPGError(int linkId);
};


#endif//__HPG_LIST_H________________________20080814110707__
