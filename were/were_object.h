#ifndef WERE_OBJECT_H
#define WERE_OBJECT_H

#include "were_object_pointer.h"
#include "were_signal.h"
#include "were_thread.h"
#include "were_registry.h"
#include <atomic>




class were_object : public were_object_base
{
public:

    ~were_object();
    were_object();

    bool collapsed() const override { return collapsed_; }

    void collapse() override;

    void access() const override
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
