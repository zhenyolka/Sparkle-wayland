#ifndef WERE_OPTIONAL_H
#define WERE_OPTIONAL_H

#include "were_exception.h"
#include <mutex>

template <typename T>
class were_optional
{
public:

    ~were_optional()
    {
        clear();
    }

    were_optional() :
        pointer_(nullptr)
    {
    }

    void set(const T &value)
    {
        mutex_.lock();
        if (pointer_ != nullptr)
            delete pointer_;
        pointer_ = new T(value);
        mutex_.unlock();
    }

    void clear()
    {
        mutex_.lock();
        if (pointer_ != nullptr)
            delete pointer_;
        pointer_ = nullptr;
        mutex_.unlock();
    }

    T &operator*() const
    {
        if (pointer_ == nullptr)
            throw were_exception(WE_SIMPLE);

        return *pointer_;
    }

    bool lock()
    {
        mutex_.lock();
        if (pointer_ == nullptr)
        {
            mutex_.unlock();
            return false;
        }
        else
        {
            return true;
        }
    }

    void unlock()
    {
        mutex_.unlock();
    }

private:
    T *pointer_;
    std::mutex mutex_;
};

#endif // WERE_OPTIONAL_H
