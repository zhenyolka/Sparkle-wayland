#include <cstdio>
#include "were_thread.h"
#include "sparkle.h"
#include "sparkle_x11.h"
#include "were_debug.h"
#include "were_signal_handler.h"
#include "were_backtrace.h"
#include <csignal>



class test
{
public:

    ~test()
    {
        were_debug::instance().stop();
    }

    test()
    {
        sparkle_ = were_object_pointer<sparkle>(new sparkle());
        sparkle_x11_ = were_object_pointer<sparkle_x11>(new sparkle_x11(sparkle_));

#if 1
        sig_ = were_object_pointer<were_signal_handler>(new were_signal_handler());

        were_debug::instance().start();

        were_object::connect(sig_, &were_signal_handler::signal, sig_, [this](uint32_t number)
        {
            if (number == SIGINT)
            {
                sig_.collapse();
                sparkle_x11_.collapse();
                sparkle_.collapse();
            }
        });
#endif
    }

private:
    were_object_pointer<sparkle> sparkle_;
    were_object_pointer<sparkle_x11> sparkle_x11_;
    were_object_pointer<were_signal_handler> sig_;
};

int main(int argc, char *argv[])
{
    were_backtrace::instance().enable();

    {
        were_object_pointer<were_thread> thread(new were_thread());
    }

    test t;

    were_thread::current_thread()->run();

    were_thread::current_thread().collapse();

    fprintf(stdout, "Done.\n");

    return 0;
}
