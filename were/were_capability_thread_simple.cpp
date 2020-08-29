#include "were_capability_thread_simple.h"
#include "were_thread.h"

extern thread_local were_slot<were_pointer<were_thread>> s_current_thread;

were_capability_thread_simple::~were_capability_thread_simple()
{
}

were_capability_thread_simple::were_capability_thread_simple() :
    thread_(s_current_thread.get())
{
}

were_pointer<were_thread> were_capability_thread_simple::thread() const
{
    return thread_;
}
