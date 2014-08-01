
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "../model/units.h"
#include "../util/parse.h"

#include "icap_geometry.h"
#include "rt_inflow.h"
#include "ucf.h"

using namespace geometry;
namespace fs = boost::filesystem;


units::Units* UCS = new units::Units(units::UnitSystem::Units_US);
ucf::Ucf* UCF = new CfsUnitsConversion();


void IcapGeometry::addRealTimeInput(std::string nodeId)
{
    boost::algorithm::to_lower(nodeId);
    std::shared_ptr<Node> node = getNode(nodeId);
    if (node == NULL)
    {
        return;
    }

    std::shared_ptr<RealTimeInflow> inflow = std::shared_ptr<RealTimeInflow>(new RealTimeInflow());

    node->clearInflowObjects();
    node->attachInflow(inflow);

    this->rtInflowMap.insert(std::pair<std::string, std::shared_ptr<RealTimeInflow>>(nodeId, inflow));
}

void IcapGeometry::setRealTimeInputFlow(std::string nodeId, var_type flow)
{
    boost::algorithm::to_lower(nodeId);
    if (this->rtInflowMap.count(nodeId) == 0)
    {
        return;
    }

    this->rtInflowMap[nodeId]->setCurrentInflow(flow);
}

var_type IcapGeometry::getRealTimeNodeHead(std::string nodeId)
{
    std::shared_ptr<Node> node = getNode(nodeId);
    if (node == NULL)
    {
        return ERROR_VAL;
    }

    return node->variable(variables::NodeDepth) + node->getInvert();
}


bool IcapGeometry::processOptions()
{
    std::vector<std::string> options = getOptionNames();

    if (!hasOption("start_date") || !hasOption("start_time") || !hasOption("end_date") || !hasOption("end_time") ||
        !hasOption("flow_units") || !hasOption("flow_units") || !hasOption("hpg_path") || !hasOption("routing_step"))
    {
        return false;
    }

    if (getOption("flow_units") != "cfs")
    {
        setErrorMessage("Invalid option value for flow_units");
        return false;
    }
    else
    {
        if (UCF != NULL)
            delete UCF;
        UCF = new CfsUnitsConversion();
    }

    if (!DateTime::tryParseDate(getOption("start_date"), this->startDate, Format::M_D_Y))
    {
        setErrorMessage("Unable to parse option start_date");
        return false;
    }

    if (!DateTime::tryParseTime(getOption("start_time"), this->startTime))
    {
        setErrorMessage("Unable to parse option start_time");
        return false;
    }
        
    if (!DateTime::tryParseDate(getOption("end_date"), this->endDate, Format::M_D_Y))
    {
        setErrorMessage("Unable to parse option end_date");
        return false;
    }

    if (!DateTime::tryParseTime(getOption("end_time"), this->startTime))
    {
        setErrorMessage("Unable to parse option end_time");
        return false;
    }

    this->hpgPath = getOption("hpg_path");
    if (!boost::filesystem::exists(this->hpgPath))
    {
        fs::path parentDir = fs::path(this->geomFilePath).parent_path();
        this->hpgPath = (parentDir / this->hpgPath).string();
        if (!boost::filesystem::exists(this->hpgPath))
        {
            setErrorMessage("Invalid path to HPG folder '" + this->hpgPath + "'");
            return false;
        }
    }

    if (!tryParse(getOption("routing_step"), this->routeStep))
    {
        setErrorMessage("Non-numeric invalid routing_step option is provided.");
        return false;
    }

    return true;
}

void IcapGeometry::startTimestep(DateTime dateTime)
{
    this->currentDateTime = dateTime;
    for (auto iter = Geometry::beginNode(); iter != Geometry::endNode(); iter++)
    {
        iter->second->startInflow(dateTime);
    }
}

var_type IcapGeometry::getNodeVariable(id_type nodeIdx, variables::Variables var)
{
    return getNode(nodeIdx)->variable(var);
}

var_type IcapGeometry::getLinkVariable(id_type linkIdx, variables::Variables var)
{
    return getLink(linkIdx)->variable(var);
}

void IcapGeometry::setNodeVariable(id_type nodeIdx, variables::Variables var, var_type value)
{
    getNode(nodeIdx)->variable(var) = value;
}

void IcapGeometry::setLinkVariable(id_type linkIdx, variables::Variables var, var_type value)
{
    getLink(linkIdx)->variable(var) = value;
}

void IcapGeometry::updateNodeStatistic(id_type nodeId, statvariables::StatVariables var, var_type value)
{
    //TODO
}

/// <summary>
/// This function updates the given system statistic by adding the given value.
/// </summary>
void IcapGeometry::updateSystemStatistic(statvariables::StatVariables var, var_type value)
{
    //TODO:
}

DateTime IcapGeometry::getCurrentDateTime()
{
    //TODO:
    return DateTime();
}

void IcapGeometry::resetDepths()
{
    //TODO:
}

void IcapGeometry::resetTimestep()
{
    for (auto iter = Geometry::beginNode(); iter != Geometry::endNode(); iter++)
    {
        iter->second->resetFlow();
        iter->second->resetDepth();
    }

    for (auto iter = Geometry::beginLink(); iter != Geometry::endLink(); iter++)
    {
        iter->second->resetFlow();
        iter->second->resetDepth();
    }
}
