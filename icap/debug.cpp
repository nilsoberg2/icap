
#include <cmath>

#include "common.h"
#include "debug.h"
#include "routing.h"


#ifdef ICAP_DEBUGGING
void OutputTimeStep(char* datetime, FILE* fh)
{
    fprintf(fh, "TIMESTEP: %s\n", datetime);
    
    for (int i = GLINK_COUNT-1; i >= 0; i--)
    {
        // Compute the maximum node depth.
        double maxDepth = GLINK_MAXDEPTH(i) * 0.80;
        
        // Output the link status.
        fprintf(fh, "%s\t%f\t%f\t%f\t", GLINK_ID(i), GLINK_FLOW(i), GLINK_DSDEPTH(i), GLINK_USDEPTH(i));
        
        // Determine if the pipe is pressurized.
        if (GLINK_USDEPTH(i) > maxDepth && GLINK_DSDEPTH(i) > maxDepth)
            fprintf(fh, "Pressurized");
        
        // Else, determine if part of the pipe is pressurized.
        else if ((GLINK_USDEPTH(i) > maxDepth && GLINK_DSDEPTH(i) <= maxDepth) ||
                 (GLINK_USDEPTH(i) <= maxDepth && GLINK_DSDEPTH(i) > maxDepth))
            fprintf(fh, "Transitional");
        
        // No-flow case
        else if (IS_ZERO(GLINK_FLOW(i)))
        {
            if (GLINK_USDEPTH(i) > 0.0)
                fprintf(fh, "Ponded");
            else
                fprintf(fh, "Empty");
        }

        // Final condition is open channel flow.
        else if (GLINK_USDEPTH(i) <= maxDepth && GLINK_DSDEPTH(i) <= maxDepth)
            fprintf(fh, "Open-channel");

        fprintf(fh, "\n");
    }
}
#endif
