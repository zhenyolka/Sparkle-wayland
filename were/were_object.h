#ifndef WERE_OBJECT_H
#define WERE_OBJECT_H

#include "were_exception.h"
#include "were_signal.h"
#include <cstdio>
#include <typeinfo>
#include <functional>

#define MAKE_THIS_WOP \
were_object_pointer<std::remove_pointer<decltype(this)>::type> this_wop(this);

/* ================================================================================================================== */

class were_object;
class were_thread;

template <typename T>
class were_object_pointer
{
    template <typename T2>
    friend class were_object_pointer;

public:
    ~were_object_pointer();
    were_object_pointer();
    were_object_pointer(T *object__);
    were_object_pointer(const were_object_pointer &other);
    template <typename T2>
    were_object_pointer(const were_object_pointer<T2> &other);
    were_object_pointer &operator=(const were_object_pointer &other);
    void reset();
    void collapse();
    T *access() const;
    T *access_UNSAFE() const;
    T *operator->() const;
    operator bool() const;
    bool operator==(const were_object_pointer &other) const;
    bool operator!=(const were_object_pointer &other) const;
    void increment_reference_count();
    void decrement_reference_count();
    int reference_count();
    were_object_pointer<were_object> were() const;
    operator were_object_pointer<were_object>();
    were_object_pointer<were_thread> thread() const;
    void post(const std::function<void ()> &call);

private:
    were_object *object_;
    T *pointer_;
};

/* ================================================================================================================== */

class were_object
{
public:

    virtual ~were_object();
    were_object();

    void increment_reference_count()
    {
        reference_count_ += 1;
    }

    void decrement_reference_count()
    {
        reference_count_ -= 1;
    }

    int reference_count() const
    {
        return reference_count_;
    }

    bool collapsed() const
    {
        return collapsed_;
    }

    void collapse();

    were_object_pointer<were_thread> thread() const
    {
        return thread_;
    }

    void add_dependency(were_object_pointer<were_object> dependency);

    static uint64_t next_id()
    {
        return next_id_++;
    }

    template <typename SourceType, typename SignalType, typename Functor>
    static void connect(    were_object_pointer<SourceType> source,
                            SignalType signal,
                            were_object_pointer<were_object> context,
                            Functor call);

    template <typename SourceType, typename SignalType>
    static void disconnect( were_object_pointer<SourceType> source,
                            SignalType signal,
                            were_object_pointer<were_object> context,
                            uint64_t pc_id, uint64_t sb_id, uint64_t cb_id);

    template <typename SourceType, typename SignalType, typename ...Args>
    static void emit(   were_object_pointer<SourceType> source,
                        SignalType signal,
                        Args... args);

    bool same_thread() const;
    void post(const std::function<void ()> &call);

signals:
    were_signal<void ()> destroyed;

private:
    int reference_count_;
    bool collapsed_;
    were_object_pointer<were_thread> thread_;
    static uint64_t next_id_;
};

/* ================================================================================================================== */

template <typename T>
were_object_pointer<T>::~were_object_pointer()
{
    reset();
}

template <typename T>
were_object_pointer<T>::were_object_pointer()
{
    object_ = nullptr;
    pointer_ = nullptr;
}

template <typename T>
were_object_pointer<T>::were_object_pointer(T *object__)
{
    object_ = object__;
    pointer_ = object__;

    if (object_ != nullptr)
        object_->increment_reference_count();

    if (object_ != nullptr && object_->collapsed()) // XXXT
    {
        fprintf(stdout, "Type %s\n", typeid(T).name());
        throw were_exception(WE_SIMPLE);
    }
}

template <typename T>
were_object_pointer<T>::were_object_pointer(const were_object_pointer &other)
{
    object_ = other.object_;
    pointer_ = other.pointer_;

    if (object_ != nullptr)
        object_->increment_reference_count();
}

template <typename T>
template <typename T2>
were_object_pointer<T>::were_object_pointer(const were_object_pointer<T2> &other)
{
    object_ = other.object_;
    pointer_ = other.pointer_;

    if (object_ != nullptr)
        object_->increment_reference_count();
}

template <typename T>
were_object_pointer<T> &were_object_pointer<T>::operator=(const were_object_pointer &other)
{
    reset();

    object_ = other.object_;
    pointer_ = other.pointer_;

    if (object_ != nullptr)
        object_->increment_reference_count();

    return *this;
}

template <typename T>
void were_object_pointer<T>::reset()
{
    if (object_ != nullptr)
    {
        object_->decrement_reference_count();

        if (object_->reference_count() == 0 && object_->collapsed())
        {
            if (object_->same_thread())
            {
                delete object_;
            }
            else
            {
                were_object *object__ = object_;
                object_->post([object__](){delete object__;});
            }
        }

        object_ = nullptr;
        pointer_ = nullptr;
    }
}

template <typename T>
void were_object_pointer<T>::collapse()
{
    if (object_ != nullptr)
    {
        object_->collapse();
        reset();
    }
}

template <typename T>
T *were_object_pointer<T>::access() const
{
    if (pointer_ == nullptr)
    {
        fprintf(stdout, "Type %s\n", typeid(T).name());
        throw were_exception(WE_SIMPLE);
    }

    if (!object_->same_thread())
    {
        fprintf(stdout, "Type %s\n", typeid(T).name());
        throw were_exception(WE_SIMPLE);
    }

    return pointer_;
}

template <typename T>
T *were_object_pointer<T>::access_UNSAFE() const
{
    if (pointer_ == nullptr)
    {
        fprintf(stdout, "Type %s\n", typeid(T).name());
        throw were_exception(WE_SIMPLE);
    }

    return pointer_;
}

template <typename T>
T *were_object_pointer<T>::operator->() const
{
    return access();
}

template <typename T>
were_object_pointer<T>::operator bool() const
{
    return pointer_ != nullptr;
}

template <typename T>
bool were_object_pointer<T>::operator==(const were_object_pointer &other) const
{
    return pointer_ == other.pointer_;
}

template <typename T>
bool were_object_pointer<T>::operator!=(const were_object_pointer &other) const
{
    return pointer_ != other.pointer_;
}

template <typename T>
void were_object_pointer<T>::increment_reference_count()
{
    object_->increment_reference_count();
}

template <typename T>
void were_object_pointer<T>::decrement_reference_count()
{
    object_->decrement_reference_count();
}

template <typename T>
int were_object_pointer<T>::reference_count()
{
    return object_->reference_count();
}

template <typename T>
were_object_pointer<were_object> were_object_pointer<T>::were() const
{
    if (object_ == nullptr)
    {
        fprintf(stdout, "Type %s\n", typeid(T).name());
        throw were_exception(WE_SIMPLE);
    }

    return were_object_pointer<were_object>(object_);
}

template <typename T>
were_object_pointer<T>::operator were_object_pointer<were_object>()
{
    return were_object_pointer<were_object>(object_);
}

template <typename T>
were_object_pointer<were_thread> were_object_pointer<T>::thread() const
{
    return object_->thread();
}

template <typename T>
void were_object_pointer<T>::post(const std::function<void ()> &call)
{
    object_->post(call);
}

/* ================================================================================================================== */

template <typename SourceType, typename SignalType, typename Functor>
void were_object::connect(  were_object_pointer<SourceType> source,
                            SignalType signal,
                            were_object_pointer<were_object> context,
                            Functor call
)
{
    uint64_t pc_id = next_id();
    uint64_t sb_id = next_id();
    uint64_t cb_id = next_id();

    std::function<void ()> breaker = [source, signal, context, pc_id, sb_id, cb_id]()
    {
        were_object::disconnect(source, signal, context, pc_id, sb_id, cb_id);
    };

    source.post([source, signal, call, pc_id]()
    {
        auto signal__ = &((source.access())->*signal);
        signal__->add_connection(call, pc_id);
    });

    source.post([source, breaker, sb_id]()
    {
        auto signal__ = &((source.access())->destroyed);
        signal__->add_connection(breaker, sb_id);
    });

    context.post([context, breaker, cb_id]()
    {
        auto signal__ = &((context.access())->destroyed);
        signal__->add_connection(breaker, cb_id);
    });
};

template <typename SourceType, typename SignalType>
void were_object::disconnect(   were_object_pointer<SourceType> source,
                                SignalType signal,
                                were_object_pointer<were_object> context,
                                uint64_t pc_id, uint64_t sb_id, uint64_t cb_id)
{
    source.post([source, signal, pc_id]()
    {
        auto signal__ = &((source.access())->*signal);
        signal__->remove_connection(pc_id);
    });

    source.post([source, sb_id]()
    {
        auto signal__ = &((source.access())->destroyed);
        signal__->remove_connection(sb_id);
    });

    context.post([context, cb_id]()
    {
        auto signal__ = &((context.access())->destroyed);
        signal__->remove_connection(cb_id);
    });
}

template <typename SourceType, typename SignalType, typename ...Args>
void were_object::emit( were_object_pointer<SourceType> source,
                        SignalType signal,
                        Args... args
)
{
    auto signal__ = &((source.access())->*signal);
    signal__->emit(args...);
};

/* ================================================================================================================== */

#endif // WERE_OBJECT_H
