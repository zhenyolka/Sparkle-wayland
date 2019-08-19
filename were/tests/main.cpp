#include <cstdio>
#include "were_object.h"
#include "were_thread.h"
#include "were_timer.h"


class someint
{
public:
    virtual void e() = 0;
};

#if 1
class test : public were_object, public someint
{
public:
    ~test()
    {
    }
    test()
    {
        MAKE_THIS_WOP

        were_object_pointer<were_timer> timer(new were_timer(1000));
        were_object::connect(timer, &were_timer::timeout, this_wop, [this_wop](){this_wop->timeout();});
        timer->start();
    }

    void timeout()
    {
        fprintf(stdout, "timeout\n");
    }

    void e()
    {
        fprintf(stdout, "e\n");
    }
};
#endif



int main(int argc, char *argv[])
{
    {
        were_object_pointer<were_thread> thread(new were_thread());
    }

    were_object_pointer<test> t1(new test());
    were_object_pointer<someint> t2 = t1;
    t2->e();
    were_object::connect(t1, &were_object::destroyed, t1, [](){fprintf(stdout, "destr1\n");});
    were_object::connect(t2.were(), &were_object::destroyed, t2.were(), [](){fprintf(stdout, "destr2\n");});


    were_thread::current_thread()->run();

    fprintf(stdout, "finished\n");

    return 0;
}
