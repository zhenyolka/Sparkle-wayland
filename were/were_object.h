#ifndef WERE_OBJECT_H
#define WERE_OBJECT_H

#include "were_capability_rc.h"
#include "were_capability_collapse.h"
#include "were_capability_thread.h"
#include "were_capability_sentinel.h"
#include "were_capability_debug.h"
#include <atomic>



class were_object :
    virtual public were_capability_rc,
    virtual public were_capability_collapse,
    virtual public were_capability_thread,
    //virtual public were_capability_sentinel,
    virtual public were_capability_debug
{
public:

    virtual ~were_object();
    were_object();

    bool collapsed() const override { return collapsed_; }

    void collapse() override ;

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

    were_pointer<were_thread> thread() const override;

    //bool sentinel() const override;

    void post(const std::function<void ()> &call) override;

    void link(were_pointer<were_object> other);

    std::string dump() const override;

private:
    std::atomic<int> reference_count_;
    std::atomic<bool> collapsed_;
    were_pointer<were_thread> thread_;

};


#endif // WERE_OBJECT_H
