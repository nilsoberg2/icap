
#include <cmath>

#include "common.h"
#include "debug.h"
#include "routing.h"


#ifdef ICAP_DEBUGGING
void OutputTimeStep(char* datetime, FILE* fh)
{
    fprintf(fh, "TIMESTEP: %s\n", datetime);
    
    for (int i = Nobjects[LINK]-1; i >= 0; i--)
    {
        // Compute the maximum node depth.
        double maxDepth = LINK_DIAMETER(i) * 0.80;
        
        // Output the link status.
        fprintf(fh, "%s\t%f\t%f\t%f\t", Link[i].ID, Link[i].newFlow, Link[i].oldDepth, Link[i].newDepth);
        
        // Determine if the pipe is pressurized.
        if (Link[i].newDepth > maxDepth && Link[i].oldDepth > maxDepth)
            fprintf(fh, "Pressurized");
        
        // Else, determine if part of the pipe is pressurized.
        else if ((Link[i].newDepth > maxDepth && Link[i].oldDepth <= maxDepth) ||
                 (Link[i].newDepth <= maxDepth && Link[i].oldDepth > maxDepth))
            fprintf(fh, "Transitional");
        
        // No-flow case
        else if (IS_ZERO(Link[i].newFlow))
        {
            if (Link[i].newDepth > 0.0)
                fprintf(fh, "Ponded");
            else
                fprintf(fh, "Empty");
        }

        // Final condition is open channel flow.
        else if (Link[i].newDepth <= maxDepth && Link[i].oldDepth <= maxDepth)
            fprintf(fh, "Open-channel");

        fprintf(fh, "\n");
    }
}
#endif
