#include "were_capability_thread_simple.h"
#include "were_thread.h"

were_capability_thread_simple::~were_capability_thread_simple()
{
}

were_capability_thread_simple::were_capability_thread_simple() :
    thread_(were_t_l_slot<were_pointer<were_thread>>::get())
{
}

were_pointer<were_thread> were_capability_thread_simple::thread() const
{
    return thread_;
}
