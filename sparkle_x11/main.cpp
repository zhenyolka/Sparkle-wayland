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
#include "were_log.h"



class test : virtual public were_object
{
public:

    ~test()
    {

    }

    test()
    {
        auto this_wop = were_pointer(this);

        //were_pointer<were_log> logger(new were_log());
        //logger->link(this_wop);
        //logger->capture_stdout();
        //logger->enable_stdout();
        //logger->enable_file("/tmp/sparkle.log");
        //global_set<were_log>(logger);


        were_pointer<sparkle_settings> settings(new sparkle_settings("./sparkle.config"));
        settings->link(this_wop);
        settings->load();
        global_set<sparkle_settings>(settings);


        were_pointer<sparkle> sparkle__(new sparkle());
        sparkle__->link(this_wop);

        were_pointer<were_x11_compositor> compositor__(new were_x11_compositor());
        compositor__->link(this_wop);

        were::connect(sparkle__->shell(), &sparkle_global<sparkle_shell>::instance, compositor__, [compositor__, this_wop](were_pointer<sparkle_shell> shell)
        {
            compositor__->register_producer(shell);
        });

        were_pointer<were_signal_handler> sh(new were_signal_handler());
        sh->link(this_wop);
        were::connect(sh, &were_signal_handler::signal, this_wop, [this_wop](uint32_t number) mutable
        {
            if (number == SIGINT)
            {
                this_wop->collapse();
                t_l_global<were_thread>()->exit();
            }
        });

    }
};

int main(int argc, char *argv[])
{
    were_backtrace backtrace;
    backtrace.enable();

    were_debug *debug = new were_debug();
    were_registry<were_debug *>::set(debug);

    were_t_l_registry<were_pointer<were_thread>>::set(
        were_pointer<were_thread>(new were_thread()));

    {
        were_pointer<test> t(new test());
    }

    debug->start();

    t_l_global<were_thread>()->run();

    t_l_global<were_thread>()->run_for(1000);

    debug->stop();

    were_registry<were_debug *>::clear();
    delete debug;


    fprintf(stdout, "Done.\n");

    return 0;
}
