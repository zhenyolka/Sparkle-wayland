#ifndef WERE_OBJECT_WRAPPER_H
#define WERE_OBJECT_WRAPPER_H

#include "were_object.h"

template <typename T>
class were_object_wrapper_primitive
{

    typedef void (*destructor_type)(T &primitive);

public:

    ~were_object_wrapper_primitive()
    {
        if (destructor_ != nullptr)
            destructor_(primitive_);
    }

    were_object_wrapper_primitive(T primitive) :
        primitive_(primitive), destructor_(nullptr)
    {
    }

    void set_destructor(destructor_type destructor)
    {
        destructor_ = destructor;
    }

    T &get()
    {
        return primitive_;
    }

private:
    T primitive_;
    destructor_type destructor_;
};

template <typename T>
class were_object_wrapper : public were_object, public T
{
    using T::T;
};

#endif // WERE_OBJECT_WRAPPER_H
