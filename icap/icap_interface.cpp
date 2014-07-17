
// INTERFACE SO THAT GUI CAN COMMUNICATE WITH ICAP

#include "icap.h"
#include "icap_interface.h"

extern "C"
{

int __stdcall NewICAP()
{
    return (int)(new ICAP());
}


int __stdcall DeleteICAP(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle != NULL)
        delete handle;
    return 0;
}


int __stdcall icap_count_hpg(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    return handle->GetLinkCount();
}


int __stdcall icap_load_hpg_step(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    return handle->loadNextHpg();
}


int __stdcall icap_open(int h, char* inputFile, char* reportFile, char* outputFile)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;
    
    bool result = handle->Open(inputFile, reportFile, outputFile, true);  // true to load HPGs
    return (result ? 0 : 1);
}


int __stdcall icap_open_no_hpg(int h, char* inputFile, char* reportFile, char* outputFile)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;
    
    bool result = handle->Open(inputFile, reportFile, outputFile, false);  // false to not load HPGs
    return (result ? 0 : 1);
}


int __stdcall icap_start(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;
    
    bool result = handle->Start();
    return (result ? 0 : 1);
}


int __stdcall icap_end(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;
    
    bool result = handle->End();
    return (result ? 0 : 1);
}


int __stdcall icap_close(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;
    
    bool result = handle->Close();
    return (result ? 0 : 1);
}


int __stdcall icap_step(int h, double* curStep_in)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;

    bool result = handle->Step(curStep_in, false);
    return (result ? 0 : 1);
}


int __stdcall icap_step_dt(int h, double dt, double* elapsedTime)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;

    bool result = handle->Step(elapsedTime, dt, false);
    return (result ? 0 : 1);
}


// enable real-time mode, where results can be set and queried
int __stdcall icap_enable_rt_mode(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;

    handle->SetRealTimeStatus(true);
    
    return 0;
}


// disable real-time mode, and just execute from input file
int __stdcall icap_disable_rt_mode(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return 1;

    handle->SetRealTimeStatus(false);

    return 0;
}


int __stdcall icap_clear_node_flows(int h)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return ERROR_VAL;

    handle->InitializeZeroFlows();

    return 0;
}

int __stdcall icap_set_node_flow(int h, char* nodeId, double flow)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return ERROR_VAL;

    handle->SetCurrentNodeInflow(nodeId, flow);
}


double __stdcall icap_get_node_head(int h, char* nodeId)
{
	ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return ERROR_VAL;

    return handle->GetCurrentNodeHead(nodeId);
}


int __stdcall icap_add_source(int h, char* nodeId)
{
    ICAPHandle handle = (ICAPHandle)h;
    if (handle == NULL)
        return ERROR_VAL;

    handle->AddSource(nodeId);

    return 0;
}


} // extern "C"
