#include <cstdio>
#include "were_thread.h"
#include "were_timer.h"
#include "sparkle.h"


int main(int argc, char *argv[])
{
#if TEST
    were_object_pointer<were_thread> thread(new were_thread());
    thread->post([](){fprintf(stdout, "Test!\n");});

    were_object_pointer<were_timer> timer(new were_timer(500));
    timer->start();

    were::connect(timer, &were_timer::timeout, timer, [](){fprintf(stdout, "x\n");});

#if 0
    thread->run();
#else
    for (int i = 0; i < 20; ++i)
    {
        thread->process(1000);
        fprintf(stdout, "Processing (%d), rc %d.\n", i, thread->reference_count());

        if (i == 5)
            timer.collapse();
    }
#endif

    thread.collapse();


    return 0;
#else

    were_object_pointer<were_thread> thread(new were_thread());
    were_object_pointer<sparkle> sparkle1(new sparkle());

    thread->run();

    sparkle1.collapse();
    thread.collapse();

#endif
}
