#include <cstdio>
#include "were_object.h"
#include "were_thread.h"
#include "were_timer.h"
#include "were_x11_surface_provider.h"
#include "were_surface.h"
#include "were_backtrace.h"

#define WIDTH 1280
#define HEIGHT 720

class test : public were_object
{
public:

    ~test()
    {
    }

    test()
    {
        MAKE_THIS_WOP

        provider_ = were_object_pointer<were_x11_surface_provider>(new were_x11_surface_provider());


        were_platform_surface_provider::set_default_provider(provider_);

        surface_ = were_object_pointer<were_surface>(new were_surface(WIDTH, HEIGHT, WERE_SURFACE_FORMAT_ARGB8888));

        were_object_pointer<were_timer> tmr(new were_timer(1000 / 30));
        were_object::connect(tmr, &were_timer::timeout, this_wop, [this_wop]()
        {
            this_wop->timeout();
        });
        tmr->start();
    }

    void timeout()
    {
        static int x = 0;
        x += 1;

        char *data;

        int x1 = 0;
        int y1 = 0;
        int x2 = WIDTH;
        int y2 = HEIGHT;
        int stride;

        surface_->lock(&data, &x1, &y1, &x2, &y2, &stride);
        memset(data, x % 256, WIDTH * HEIGHT * 4);
        surface_->unlock_and_post();
    }

private:
    were_object_pointer<were_x11_surface_provider> provider_;
    were_object_pointer<were_surface> surface_;
};


int main(int argc, char *argv[])
{
    were_backtrace::instance().enable();

    {
        were_object_pointer<were_thread> t(new were_thread());
    }


    were_object_pointer<test> test1(new test());

    were_thread::current_thread()->run();

    return 0;
}
