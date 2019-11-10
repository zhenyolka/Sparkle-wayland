#include "were_thread.h"
#include "were_x11_surface_provider.h"
#include "sparkle.h"
#include "sparkle_platform.h"
#include "were_backtrace.h"
#include "were_debug.h"
#include "were_signal_handler.h"
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

        were_object_pointer<were_x11_surface_provider> provider__(new were_x11_surface_provider());
        were_platform_surface_provider::set_default_provider(provider__);
        were_object::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
        {
            were_platform_surface_provider::default_provider().collapse();
        });

        were_object_pointer<sparkle> sparkle__(new sparkle());
        sparkle__->link(this_wop);

        were_object_pointer<sparkle_platform> sparkle_platform__(new sparkle_platform(sparkle__));
        sparkle_platform__->link(this_wop);
    }
};

int main(int argc, char *argv[])
{
    were_backtrace::instance().enable();

    {
        were_object_pointer<were_thread> thread(new were_thread());
    }

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

    were_debug::instance().start();

    were_thread::current_thread()->run();

    were_thread::current_thread().collapse();

    were_debug::instance().stop();

    fprintf(stdout, "Done.\n");

    return 0;
}
