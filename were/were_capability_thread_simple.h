#ifndef WERE_CAPABILITY_THREAD_SIMPLE_H
#define WERE_CAPABILITY_THREAD_SIMPLE_H

#include "were_capability_thread.h"

class were_capability_thread_simple : public were_capability_thread
{
public:
    ~were_capability_thread_simple();
    were_capability_thread_simple();

    were_pointer<were_thread> thread() const override;

private:
    were_pointer<were_thread> thread_;
};

#endif // WERE_CAPABILITY_THREAD_SIMPLE_H
