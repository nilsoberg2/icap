#ifndef __ICAP_INTERFACE_H__________________20080806160505__
#define __ICAP_INTERFACE_H__________________20080806160505__

#if defined(ICAP_EXPORT_DLL)
    #define ICAPDLLEXPORT __declspec(dllexport)
#else
    #if defined(ICAP_IMPORT_DLL)
        #define ICAPDLLEXPORT __declspec(dllimport)
    #else
        #define ICAPDLLEXPORT
    #endif
#endif


typedef ICAP* ICAPHandle;

extern "C"
{

ICAPDLLEXPORT int __stdcall icap_open(int handle, char* inputFile, char* reportFile, char* outputFile);
ICAPDLLEXPORT int __stdcall icap_open_no_hpg(int handle, char* inputFile, char* reportFile, char* outputFile);
ICAPDLLEXPORT int __stdcall icap_start(int handle);
ICAPDLLEXPORT int __stdcall icap_step(int handle, double* curStep_in);
ICAPDLLEXPORT int __stdcall icap_end(int handle);
ICAPDLLEXPORT int __stdcall icap_close(int handle);

ICAPDLLEXPORT int __stdcall icap_count_hpg(int handle);
ICAPDLLEXPORT int __stdcall icap_load_hpg_step(int handle); // 0 no error, > 0 error, < 0 finished

ICAPDLLEXPORT int __stdcall icap_enable_rt_mode(int handle); // enable real-time mode, where results can be set and queried
ICAPDLLEXPORT int __stdcall icap_disable_rt_mode(int handle); // disable real-time mode, and just execute from input file

ICAPDLLEXPORT int __stdcall icap_clear_node_flows(int handle);
ICAPDLLEXPORT int __stdcall icap_add_source(int handle, char* nodeId);
ICAPDLLEXPORT int __stdcall icap_set_node_flow(int handle, char* nodeId, double flow);
ICAPDLLEXPORT double __stdcall icap_get_node_head(int handle, char* nodeId);

ICAPDLLEXPORT int __stdcall NewICAP();
int ICAPDLLEXPORT __stdcall DeleteICAP(int handle);

} // extern "C"

#endif//__ICAP_INTERFACE_H__________________20080806160505__
