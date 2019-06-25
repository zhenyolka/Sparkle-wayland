#include <cstdio>
#include "were_thread.h"
#include "were_debug.h"
#include "were_object_2.h"


class c1 : public were_object_2
{
public:
signals:
    were_signal<void ()> sig;
};

class c2 : public were_object_2
{
public:
    void slot()
    {
        fprintf(stdout, "Slot!\n");
    }
};


int main(int argc, char *argv[])
{
    were_object_pointer<were_thread> thread(new were_thread());
    were_object_pointer<were_debug> debug(new were_debug());



    were_object_pointer<c1> c1_(new c1());
    were_object_pointer<c2> c2_(new c2());
    were::connect(c1_, &c1::sig, c2_, [c2_](){c2_->slot();});

    for (int i = 0; ; ++i)
    {
        fprintf(stdout, "Processing %d\n", i);
        if (c1_)
            were::emit(c1_, &c1::sig);
        thread->process(1000);


        if (i == 5)
        {
            fprintf(stdout, "Shutting down\n");
            c1_.collapse();
        }
    }

    debug.collapse();
    thread.collapse();

    return 0;
}
