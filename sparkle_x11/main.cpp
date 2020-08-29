#include "were_thread.h"
#include "were_handler.h"
#include "sparkle.h"
#include "were_backtrace.h"
#include "were_debug.h"
#include "were_signal_handler.h"
#include "were_x11_compositor.h"
#include "sparkle_global.h"
#include "sparkle_shell.h"
#include <csignal>
#include <cstdio>
#include "were_slot.h"
#include "were_settings.h"
#include "were_log.h"
#include "were_fd.h"
#include <unistd.h>


#define STDOUT_CAP


were_slot<were_pointer<were_settings>> s_settings;
thread_local were_slot<were_pointer<were_thread>> s_current_thread;
were_slot<were_log *> s_log;
were_slot<were_debug *> s_debug;


class sparkle_x11 : virtual public were_object
{
public:

    ~sparkle_x11() override
    {
        s_settings.get().collapse();
        s_settings.clear();
    }

    sparkle_x11()
    {
        add_integrator([this]()
        {
            auto this_wop = were_pointer(this);

            were_pointer<were_settings> settings = were_new<were_settings>("./sparkle.config");
            //were::link(settings, this_wop);
            settings->load();
            s_settings.set(settings);


            were_pointer<sparkle> sparkle__ = were_new<sparkle>();
            were::link(sparkle__, this_wop);

            were_pointer<were_x11_compositor> compositor__ = were_new<were_x11_compositor>();
            were::link(compositor__, this_wop);

            were::connect(sparkle__->shell(), &sparkle_global<sparkle_shell>::instance, compositor__, [compositor__, this_wop](were_pointer<sparkle_shell> shell)
            {
                compositor__->register_producer(shell);
            });

            were_pointer<were_signal_handler> sh = were_new<were_signal_handler>();
            were::link(sh, this_wop);
            were::connect(sh, &were_signal_handler::signal, this_wop, [this_wop](uint32_t number)
            {
                if (number == SIGINT)
                {
                    //this_wop.collapse();
                    s_current_thread.get()->exit();
                }
            });
        });
    }

};

int main(int argc, char *argv[])
{
    were_log *logger = new were_log();
    logger->enable_fd(dup(fileno(stdout)));
    s_log.set(logger);

    were_backtrace backtrace;
    backtrace.enable();

    were_debug *debug = new were_debug();
    s_debug.set(debug);


    {
        were_pointer<were_thread> thread = were_new<were_thread>();
        s_current_thread.set(thread);

        were_pointer<were_handler> handler = were_new<were_handler>();
        thread->set_handler(handler);

#ifdef STDOUT_CAP
        were_pointer<were_fd> log_redirect = were_new<were_fd>(stdout_capture(), EPOLLIN);
        were::connect(log_redirect, &were_fd::destroyed, log_redirect, [log_redirect]()
        {
            stdout_restore();
        });
        were::connect(log_redirect, &were_fd::data_in, log_redirect, [log_redirect]()
        {
            std::vector<char> buffer = log_redirect->read(512);
            log(buffer);
        });
#endif

        were_pointer<sparkle_x11> sparkle__ = were_new<sparkle_x11>();

        debug->start(); // After sigint blocked


        thread->run();

        sparkle__.collapse();

#ifdef STDOUT_CAP
        log_redirect.collapse();
#endif

        thread->run_for(1000);

        thread->unset_handler();
        handler.collapse();
        thread.collapse();

        thread->run_for(1000);

        s_current_thread.clear();
    }

    debug->stop();
    s_debug.clear();
    delete debug;

    s_log.clear();
    delete logger;


    fprintf(stdout, "Done.\n");

    return 0;
}
