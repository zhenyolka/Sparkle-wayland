#include "were_object_1.h"
#include "were_exception.h"
#include "were_thread.h"

were_object_1::were_object_1()
{
    thread_ = were_thread::current_thread();

    if (!thread_)
        throw were_exception(WE_SIMPLE);
}
