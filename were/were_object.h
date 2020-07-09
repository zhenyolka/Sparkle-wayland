#ifndef WERE_OBJECT_H
#define WERE_OBJECT_H

#include "were_capability_rc.h"
#include "were_capability_collapse.h"
#include "were_capability_thread.h"
#include "were_capability_debug.h"
#include <atomic>


class were_object :
    virtual public were_capability_rc,
    virtual public were_capability_collapse,
    virtual public were_capability_thread,
    virtual public were_capability_debug,
    virtual public were_capability_managed
{
public:

    virtual ~were_object();
    were_object();

    bool collapsed() const override { return collapsed_; }
    void collapse() override ;
    void reference() override;
    void unreference() override;
    int reference_count() const override { return reference_count_.load(); }
    were_pointer<were_thread> thread() const override;
    std::string dump() const override;

private:
    std::atomic<int> reference_count_;
    std::atomic<bool> collapsed_;
    were_pointer<were_thread> thread_;

};


#endif // WERE_OBJECT_H
