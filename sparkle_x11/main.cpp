#include "were_thread.h"
#include "sparkle.h"
#include "were_backtrace.h"
#include "were_debug.h"
#include "were_signal_handler.h"
#include "were_x11_compositor.h"
#include "sparkle_global.h"
#include "sparkle_shell.h"
#include <csignal>
#include <cstdio>
#include "were_registry.h"
#include "sparkle_settings.h"



class test : public were_object
{
public:

    ~test()
    {

    }

    test()
    {
        auto this_wop = make_wop(this);

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

    were_debug *debug = new were_debug();
    were_registry<were_debug *>::set(debug);

    were_t_l_registry<were_object_pointer<were_thread>>::set(
        were_object_pointer<were_thread>(new were_thread()));

    were_t_l_registry<were_object_pointer<sparkle_settings>>::set(
        were_object_pointer<sparkle_settings>(new sparkle_settings()));
    were_t_l_registry<were_object_pointer<sparkle_settings>>::get()->load("./sparkle.config");

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

    debug->start();

    t_l_global<were_thread>()->run();

    debug->stop();

    were_registry<were_debug *>::clear();
    delete debug;


    fprintf(stdout, "Done.\n");

    return 0;
}
