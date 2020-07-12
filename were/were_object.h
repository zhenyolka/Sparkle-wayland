#ifndef WERE_OBJECT_H
#define WERE_OBJECT_H

#include "were_capability_rc_simple.h"
#include "were_capability_collapse.h"
#include "were_capability_thread_simple.h"
#include "were_capability_debug.h"


class were_object : virtual public were_capability_rc_simple,
                    virtual public were_capability_integrator,
                    virtual public were_capability_collapse,
                    virtual public were_capability_thread_simple,
                    virtual public were_capability_debug
{
public:
    std::string dump() const override;
};

#endif // WERE_OBJECT_H
