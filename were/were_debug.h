#ifndef WERE_DEBUG_H
#define WERE_DEBUG_H

#include "were_object.h"
#include <ctime>

class were_timer;

class were_debug : public were_object
{
public:
    ~were_debug();
    were_debug();

private:
    void timeout();

private:
    were_object_pointer<were_timer> timer_;
    struct timespec real1_, real2_;
    struct timespec cpu1_, cpu2_;
};

#endif // WERE_DEBUG_H
