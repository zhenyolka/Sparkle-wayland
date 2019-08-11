#ifndef WERE_DEBUG_H
#define WERE_DEBUG_H

#include <ctime>

class were_debug
{
public:
    ~were_debug();
    were_debug();

    void process();

private:
    struct timespec real1_, real2_;
    struct timespec cpu1_, cpu2_;
};

#endif // WERE_DEBUG_H
