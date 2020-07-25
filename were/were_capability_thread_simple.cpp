#include "were_capability_thread_simple.h"
#include "were_global.h"
#include "were_thread.h"

were_capability_thread_simple::~were_capability_thread_simple()
{
}

were_capability_thread_simple::were_capability_thread_simple() :
    thread_(t_l_slot<were_thread>())
{
}

were_pointer<were_thread> were_capability_thread_simple::thread() const
{
    return thread_;
}
