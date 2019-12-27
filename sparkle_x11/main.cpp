#include "were_thread.h"
#include "sparkle.h"
#include "were_backtrace.h"
#include "were_debug.h"
#include "were_signal_handler.h"
#include "were_x11_compositor.h"
#include "sparkle_global.h"
#include "sparkle_shell.h"
#include "were_registry.h"
#include <csignal>
#include <cstdio>



class test : public were_object
{
public:

    ~test()
    {

    }

    test()
    {
        MAKE_THIS_WOP

        were_object_pointer<sparkle> sparkle__(new sparkle());
        sparkle__->link(this_wop);

        were_object_pointer<were_x11_compositor> compositor__(new were_x11_compositor());
        compositor__->link(this_wop);

        were_object::connect(sparkle__->shell(), &sparkle_global<sparkle_shell>::instance, compositor__, [compositor__, this_wop](were_object_pointer<sparkle_shell> shell)
        {
            compositor__->register_producer(shell);
        });
    }
};

int main(int argc, char *argv[])
{
    were_backtrace backtrace;
    backtrace.enable();

    were_debug debug;
    were_registry<were_debug>::set(&debug);

    {
        were_object_pointer<test> t(new test());


        were_object_pointer<were_signal_handler> sh(new were_signal_handler());
        were_object::connect(sh, &were_signal_handler::signal, sh, [&t, &sh](uint32_t number)
        {
            if (number == SIGINT)
            {
                sh.collapse();
                t.collapse();
            }
        });
    }

    debug.start();

    were_thread::current_thread()->run();


    debug.stop();

    fprintf(stdout, "Done.\n");

    return 0;
}
