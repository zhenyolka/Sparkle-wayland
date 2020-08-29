#ifndef WERE_SLOT_H
#define WERE_SLOT_H

#include "were_exception.h"
#include <optional>
#include <mutex>


template <typename T>
class were_slot
{
public:

    void set(const T &value)
    {
        mutex_.lock();
        value_ = value;
        mutex_.unlock();
    }

    void clear()
    {
        mutex_.lock();
        value_.reset();
        mutex_.unlock();
    }

    T &get()
    {
        if (!value_.has_value())
            throw were_exception(WE_SIMPLE);

        return value_.value();
    }

    bool lock()
    {
        mutex_.lock();

        if (!value_.has_value())
        {
            mutex_.unlock();
            return false;
        }

        return true;
    }

    void unlock()
    {
        mutex_.unlock();
    }

private:
    std::optional<T> value_;
    std::mutex mutex_;
};


#endif // WERE_SLOT_H
