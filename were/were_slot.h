#ifndef WERE_SLOT_H
#define WERE_SLOT_H

#include <optional>
#include <mutex>

template <typename T>
class were_slot
{
public:

    static void set(const T &value)
    {
        mutex_.lock();
        value_ = value;
        mutex_.unlock();
    }

    static void clear()
    {
        mutex_.lock();
        value_.reset();
        mutex_.unlock();
    }

    static T &get()
    {
        return value_.value();
    }

    static bool lock()
    {
        mutex_.lock();

        if (!value_.has_value())
        {
            mutex_.unlock();
            return false;
        }

        return true;
    }

    static void unlock()
    {
        mutex_.unlock();
    }

private:
    static std::optional<T> value_;
    static std::mutex mutex_;
};

template <typename T>
std::optional<T> were_slot<T>::value_;

template <typename T>
std::mutex were_slot<T>::mutex_;

template <typename T>
class were_t_l_slot
{
public:

    static void set(const T &value)
    {
        value_ = value;
    }

    static void clear()
    {
        value_.reset();
    }

    static T &get()
    {
        return value_.value();
    }

private:
    static thread_local std::optional<T> value_;
};

template <typename T>
thread_local std::optional<T> were_t_l_slot<T>::value_;

#endif // WERE_SLOT_H
