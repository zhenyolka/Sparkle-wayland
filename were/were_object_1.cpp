#include "were_object_1.h"
#include "were_thread.h"

were_object_1::were_object_1()
{
    thread_ = were_thread::current_thread();
}
