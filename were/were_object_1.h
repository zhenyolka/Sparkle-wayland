#ifndef WERE_OBJECT_1_H
#define WERE_OBJECT_1_H

#include "were_object.h"
#include "were_object_pointer.h"

class were_thread;

class were_object_1 : public were_object
{
public:
    were_object_1();

    were_object_pointer<were_thread> thread() const
    {
        return thread_;
    }

private:
    were_object_pointer<were_thread> thread_;
};

#endif // WERE_OBJECT_1_H
