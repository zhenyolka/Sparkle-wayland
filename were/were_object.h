#ifndef WERE_OBJECT_H
#define WERE_OBJECT_H

#include "were_capability_rc_simple.h"
#include "were_capability_collapse.h"
#include "were_capability_thread.h"
#include "were_capability_debug.h"
#include <atomic>


class were_object : virtual public were_capability_rc_simple,
                    virtual public were_capability_integrator,
                    virtual public were_capability_collapse,
                    virtual public were_capability_thread,
                    virtual public were_capability_debug
{
public:

    virtual ~were_object();
    were_object();

    were_pointer<were_thread> thread() const override;
    std::string dump() const override;

private:
    were_pointer<were_thread> thread_;
};

#endif // WERE_OBJECT_H
