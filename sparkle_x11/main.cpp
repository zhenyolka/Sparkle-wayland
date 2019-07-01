#include <cstdio>
#include "were_thread.h"
#include "were_timer.h"
#include "sparkle.h"
#include "sparkle_x11.h"
#include "were_debug.h"
#include "were_signal_handler.h"
#include <csignal>

bool first_ = true;

int main(int argc, char *argv[])
{
    were_object_pointer<were_thread> thread(new were_thread());
    were_object_pointer<sparkle> sparkle__(new sparkle());
    were_object_pointer<sparkle_x11> sparkle_x11__(new sparkle_x11(sparkle__));
    were_object_pointer<were_debug> debug(new were_debug());


#if 1

    were_object_pointer<were_signal_handler> sig(new were_signal_handler());

    were::connect(sig, &were_signal_handler::signal, sig, [&sparkle__, &sparkle_x11__, &debug, &sig](uint32_t number) mutable
    {
        if (number == SIGINT)
        {
            if (first_)
            {
                first_ = false;

                //sig.collapse();
                //debug.collapse();
                sparkle_x11__.collapse();
                sparkle__.collapse();
            }
            else
                exit(-1);
        }
    });
#endif


#if 0
    thread->run();
#else
    for (int i = 0; ; ++i)
    {
        thread->process(2000);

        if (thread->reference_count() <= 5)
            break;
    }
#endif

    sig.collapse();
    debug.collapse();
    thread.collapse();

    fprintf(stdout, "Done.\n");

    return 0;
}
