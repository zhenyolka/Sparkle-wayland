#ifndef WERE_OBJECT_H
#define WERE_OBJECT_H

#include "were_capability_rc.h"
#include "were_capability_thread.h"
#include "were_signal.h"
#include "were_thread.h"
#include "were_registry.h"
#include <atomic>




class were_object : virtual public were_capability_rc, virtual public were_capability_thread
{
public:

    virtual ~were_object();
    were_object();

    bool collapsed() const { return collapsed_; }

    void collapse();

    void access() const
    {
        if (were_t_l_registry<were_object_pointer<were_thread>>::get() != thread())
            throw were_exception(WE_SIMPLE);
    }

    void reference() override { reference_count_++; }
    void unreference() override
    {
        if (reference_count_ == 1 && collapsed_)
        {
            post([this](){ delete this; });
        }
        else
            reference_count_--;
    }
    int reference_count() const override { return reference_count_.load(); }
    were_object_pointer<were_thread> thread() const override { return thread_; }
    void post(const std::function<void ()> &call) override { thread()->post(call); }

    void link(were_object_pointer<were_object> other);

signals:
    were_signal<void ()> destroyed;

private:
    std::atomic<int> reference_count_;
    std::atomic<bool> collapsed_;
    were_object_pointer<were_thread> thread_;

};


#endif // WERE_OBJECT_H
