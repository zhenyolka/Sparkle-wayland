#ifndef WERE_OBJECT_H
#define WERE_OBJECT_H

#include "were_exception.h"
#include "were_signal.h"
//#include "were_connect.h"
//#include "were_object.h"
#include <cstdio>
#include <typeinfo>

#define MAKE_THIS_WOP \
were_object_pointer<std::remove_pointer<decltype(this)>::type> this_wop(this);

class were_object;

void were_debug_add_object(were_object *object__);
void were_debug_remove_object(were_object *object__);
int were_debug_object_count();
void were_debug_print_objects();

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
    T *operator->() const;
    T *get() const;
    operator bool() const;
    bool operator==(const were_object_pointer &other) const;
    bool operator!=(const were_object_pointer &other) const;
    void increment_reference_count();
    void decrement_reference_count();
    were_object *were() const;
    operator were_object_pointer<were_object>();

private:
    were_object *object_;
    T *pointer_;
};



class were_thread;

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

signals:
    were_signal<void ()> destroyed;

private:
    int reference_count_;
    bool collapsed_;
    were_object_pointer<were_thread> thread_;
};

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
    // XXXT Thread.

    if (object_ != nullptr)
    {
        object_->decrement_reference_count();

        if (object_->reference_count() == 0 && object_->collapsed())
                delete object_;

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
T *were_object_pointer<T>::operator->() const
{
    // XXXT Thread check.

    if (pointer_ == nullptr)
    {
        fprintf(stdout, "Type %s\n", typeid(T).name());
        throw were_exception(WE_SIMPLE);
    }

    return pointer_;
}

template <typename T>
T *were_object_pointer<T>::get() const
{
    return operator->();
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
were_object *were_object_pointer<T>::were() const
{
    if (object_ == nullptr)
    {
        fprintf(stdout, "Type %s\n", typeid(T).name());
        throw were_exception(WE_SIMPLE);
    }

    return object_;
}

template <typename T>
were_object_pointer<T>::operator were_object_pointer<were_object>()
{
    return were_object_pointer<were_object>(object_);
}

namespace were
{

uint64_t next_id();

template <typename SourceType, typename SignalType, typename ContextType>
void break_(SourceType source, SignalType signal, ContextType context, uint64_t pc_id, uint64_t sb_id, uint64_t cb_id)
{
    auto signal__ = &((source.operator->())->*signal);
    signal__->remove_connection(pc_id);

    auto signal1__ = &((source.operator->())->destroyed);
    signal1__->remove_connection(sb_id);

    auto signal2__ = &((context.operator->())->destroyed);
    signal2__->remove_connection(cb_id);
}

template <typename SourceType, typename SignalType, typename ContextType, typename Functor>
void connect(   were_object_pointer<SourceType> source,
                SignalType signal,
                were_object_pointer<ContextType> context,
                Functor call
)
{
    // XXXT Thread.

    uint64_t pc_id = next_id();
    uint64_t sb_id = next_id();
    uint64_t cb_id = next_id();

    auto signal__ = &((source.operator->())->*signal);
    signal__->add_connection(call, pc_id); // XXXT Direct.

    auto signal1__ = &((source.operator->())->destroyed);
    signal1__->add_connection([source, signal, context, pc_id, sb_id, cb_id]()
    {
        break_(source, signal, context, pc_id, sb_id, cb_id);
    }, sb_id);

    auto signal2__ = &((context.operator->())->destroyed);
    signal2__->add_connection([source, signal, context, pc_id, sb_id, cb_id]()
    {
        break_(source, signal, context, pc_id, sb_id, cb_id);
    }, cb_id);
};

static void break_x_(were_object_pointer<were_object> source, were_object_pointer<were_object> context, uint64_t pc_id, uint64_t sb_id, uint64_t cb_id)
{
    auto signal__ = &((source.were())->destroyed);
    signal__->remove_connection(pc_id);

    auto signal1__ = &((source.were())->destroyed);
    signal1__->remove_connection(sb_id);

    auto signal2__ = &((context.were())->destroyed);
    signal2__->remove_connection(cb_id);
}

template <typename Functor>
void connect_x( were_object_pointer<were_object> source,
                were_object_pointer<were_object> context,
                Functor call
)
{
    // XXXT Thread.

    uint64_t pc_id = next_id();
    uint64_t sb_id = next_id();
    uint64_t cb_id = next_id();

    auto signal__ = &((source.were())->destroyed);
    signal__->add_connection(call, pc_id); // XXXT Direct.

    auto signal1__ = &((source.were())->destroyed);
    signal1__->add_connection([source, context, pc_id, sb_id, cb_id]()
    {
        break_x_(source, context, pc_id, sb_id, cb_id);
    }, sb_id);

    auto signal2__ = &((context.were())->destroyed);
    signal2__->add_connection([source, context, pc_id, sb_id, cb_id]()
    {
        break_x_(source, context, pc_id, sb_id, cb_id);
    }, cb_id);
};

template <typename SourceType, typename SignalType, typename ...Args>
void emit(  were_object_pointer<SourceType> source,
            SignalType signal,
            Args... args
)
{
    auto signal__ = &((source.operator->())->*signal);
    signal__->emit(args...);
};

};

#endif // WERE_OBJECT_H
