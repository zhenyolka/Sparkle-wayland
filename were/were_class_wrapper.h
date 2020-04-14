#ifndef WERE_CLASS_WRAPPER_H
#define WERE_CLASS_WRAPPER_H

#include "were_capability_rc.h"


template <typename T>
class were_class_wrapper : public were_capability_rc, public T
{
public:

    virtual ~were_class_wrapper()
    {
    }

    template <typename ...Args>
    were_class_wrapper(Args... args) : T(args...), reference_count_(0)
    {
    }

    were_class_wrapper() : reference_count_(0)
    {
    }

    were_class_wrapper(const were_class_wrapper &other) : T(other), reference_count_(0)
    {
    }

    void reference() override
    {
        reference_count_ += 1;
    }

    void unreference() override
    {
        reference_count_ -= 1;

        if (reference_count_ == 0)
            delete this;
    }

    int reference_count() const override
    {
        return reference_count_;
    }


    long reference_count_;
};

#endif // WERE_CLASS_WRAPPER_H
