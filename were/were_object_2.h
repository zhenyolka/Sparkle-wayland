#ifndef WERE_OBJECT_2_H
#define WERE_OBJECT_2_H

#include "were_object_1.h"
#include "were_signal.h"
#include "were_connect.h"

class were_object_2 : public were_object_1
{
public:
    were_object_2()
    {
        destroyed.set_single_shot(true);
    }

    void collapse()
    {
        were_object_1::collapse();
        were::emit(were_object_pointer<were_object_2>(this), &were_object_2::destroyed);
    }

    template <typename DependencyType>
    void add_dependency(were_object_pointer<DependencyType> dependency)
    {
        MAKE_THIS_WOP

        were::connect(dependency, &were_object_2::destroyed, this_wop, [this_wop]() mutable
        {
            this_wop.collapse();
        });
    }

signals:
    were_signal<void ()> destroyed;
};

#endif // WERE_OBJECT_2_H
