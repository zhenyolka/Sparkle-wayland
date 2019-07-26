#ifndef WERE_OBJECT_POINTER_H
#define WERE_OBJECT_POINTER_H

#include "were_exception.h"
#include "were_object.h"
#include <cstdio>
#include <typeinfo>


template <typename T>
class were_object_pointer
{
public:

    ~were_object_pointer()
    {
        reset();
    }

    were_object_pointer()
    {
        object_ = nullptr;
    }

    were_object_pointer(T *object__)
    {
        object_ = static_cast<were_object *>(object__);

        if (object_ != nullptr)
            object_->increment_reference_count();

        if (object_ != nullptr && object_->collapsed()) // XXXT
        {
            fprintf(stdout, "Type %s\n", typeid(T).name());
            throw were_exception(WE_SIMPLE);
        }
    }

    were_object_pointer(const were_object_pointer &other)
    {
        object_ = other.object_;

        if (object_ != nullptr)
            object_->increment_reference_count();
    }

    were_object_pointer &operator=(const were_object_pointer &other)
    {
        reset();

        object_ = other.object_;

        if (object_ != nullptr)
            object_->increment_reference_count();

        return *this;
    }

    void reset()
    {
        // XXXT Thread.

        if (object_ != nullptr)
        {
            object_->decrement_reference_count();

            if (object_->reference_count() == 0 && object_->collapsed())
                delete object_;

            object_ = nullptr;
        }
    }

    void collapse()
    {
        if (object_ != nullptr)
        {
            object_->collapse();
            reset();
        }
    }

    T *operator->() const
    {
        // XXXT Thread check.

        if (object_ == nullptr)
        {
            fprintf(stdout, "Type %s\n", typeid(T).name());
            throw were_exception(WE_SIMPLE);
        }

        return static_cast<T *>(object_);
    }

    operator bool() const
    {
        return object_ != nullptr;
    }

    bool operator==(const were_object_pointer &other) const
    {
        return object_ == other.object_;
    }

    bool operator!=(const were_object_pointer &other) const
    {
        return object_ != other.object_;
    }

private:
    were_object *object_;
};

#define MAKE_THIS_WOP \
were_object_pointer<std::remove_pointer<decltype(this)>::type> this_wop(this);

#endif // WERE_OBJECT_POINTER_H
