#include "were_thread.h"
#include "sparkle.h"
#include "were_backtrace.h"
#include "were_debug.h"
#include "were_signal_handler.h"
#include "were_x11_compositor.h"
#include "sparkle_global.h"
#include "sparkle_compositor.h"
#include "sparkle_output.h"
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

        were_object::connect(sparkle__->compositor(), &sparkle_global<sparkle_compositor>::instance, compositor__, [compositor__](were_object_pointer<sparkle_compositor> compositor)
        {
            compositor__->register_producer(compositor);
        });

        were_object::connect(sparkle__->output(), &sparkle_global<sparkle_output>::instance, this_wop, [this_wop](were_object_pointer<sparkle_output> output)
        {
            int width = 1280;
            int height = 720;
            int dpi = 96;
            int mm_width = width * 254 / (dpi * 10);
            int mm_height = height * 254 / (dpi * 10);

            fprintf(stdout, "display size: %dx%d %dx%d\n", width, height, mm_width, mm_height);

            output->send_geometry(0, 0, mm_width, mm_height, 0, "Barely working solutions", "Sparkle", 0);

            if (output->version() >= WL_OUTPUT_SCALE_SINCE_VERSION)
                output->send_scale(1);

            output->send_mode(WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height, 60000);

            if (output->version() >= WL_OUTPUT_DONE_SINCE_VERSION)
                output->send_done();
        });
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
