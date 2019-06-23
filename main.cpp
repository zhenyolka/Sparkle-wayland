#include <cstdio>
#include "were_thread.h"
#include "were_timer.h"
#include "sparkle.h"
#include "sparkle_x11.h"
#include "were_debug.h"



int main(int argc, char *argv[])
{
    were_object_pointer<were_thread> thread(new were_thread());
    were_object_pointer<sparkle> sparkle__(new sparkle());
    were_object_pointer<sparkle_x11> sparkle_x11__(new sparkle_x11(sparkle__));
    were_object_pointer<were_debug> debug(new were_debug());

#if 0
    thread->run();
#else
    for (int i = 0; ; ++i)
    {
        //fprintf(stdout, "Processing... (%d)\n", i);

#if 0
        if (i == 10)
        {
            fprintf(stdout, "Shutting down...\n");
            sparkle_x11__.collapse();
            sparkle__.collapse();
        }
#endif

        thread->process();
    }
#endif

    //debug.collapse();
    thread.collapse();
}
