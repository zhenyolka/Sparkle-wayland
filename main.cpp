#include <cstdio>
#include "were_thread.h"
#include "were_timer.h"
#include "sparkle.h"
#include "were_debug.h"


int main(int argc, char *argv[])
{
    were_object_pointer<were_thread> thread(new were_thread());
    were_object_pointer<sparkle> sparkle1(new sparkle());
    were_object_pointer<were_debug> debug(new were_debug());

#if 0
    thread->run();
#else
    for (int i = 0; i < 100; ++i)
    {
        fprintf(stdout, "Processing... (%d) %d\n", i, thread->reference_count());

        if (i == 10)
        {
            fprintf(stdout, "Shutting down...\n");
            sparkle1.collapse();
        }

        thread->process();
    }
#endif

    debug.collapse();
    sparkle1.collapse();
    thread.collapse();
}
