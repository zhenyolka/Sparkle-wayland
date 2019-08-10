#include "were_object.h"

#include <set>
#include <mutex>
#include <cstdio>
#include <typeinfo>
#include "were_exception.h"
#include "were_thread.h"

int object_count_ = 0;

#ifdef X_DEBUG
std::set<were_object *> object_set_;
std::mutex object_set_mutex_;

const char *state_normal = "NORMAL";
const char *state_collapsed = "COLLAPSED";
const char *state_lost = "LOST";
#endif

void were_debug_add_object(were_object *object__)
{
#ifdef X_DEBUG
    object_set_mutex_.lock();
    object_set_.insert(object__);
    object_set_mutex_.unlock();
#endif
    object_count_ += 1;
}

void were_debug_remove_object(were_object *object__)
{
#ifdef X_DEBUG
    object_set_mutex_.lock();

    auto search = object_set_.find(object__);
    if (search == object_set_.end())
    {
        const char *state;

        if (object__->collapsed())
            state = state_collapsed;
        else if (object__->reference_count() == 0)
            state = state_lost;
        else
            state = state_normal;

        fprintf(stdout, "%-20p%-45.44s%-5d%-10s\n", object__, typeid(*object__).name(), object__->reference_count(), state);

        throw were_exception(WE_SIMPLE);
    }

    object_set_.erase(search);
    object_set_mutex_.unlock();
#endif
    object_count_ -= 1;
}

int were_debug_object_count()
{
    return object_count_;
}

void were_debug_print_objects()
{
#ifdef X_DEBUG
    object_set_mutex_.lock();

    fprintf(stdout, "%-20s%-45s%-5s%-10s\n", "Pointer", "Type", "RC", "State");

    for (auto it = object_set_.begin(); it != object_set_.end(); ++it)
    {
        were_object *object__ = (*it);
        const char *state;

        if (object__->collapsed())
            state = state_collapsed;
        else if (object__->reference_count() == 0)
            state = state_lost;
        else
            state = state_normal;

        fprintf(stdout, "%-20p%-45.44s%-5d%-10s\n", object__, typeid(*object__).name(), object__->reference_count(), state);
    }

    object_set_mutex_.unlock();
#endif
}

were_object::~were_object()
{
    were_debug_remove_object(this);
}

were_object::were_object() :
    reference_count_(0), collapsed_(false)
{
    thread_ = were_thread::current_thread();
    destroyed.set_single_shot(true);

    were_debug_add_object(this);
}

void were_object::collapse()
{
    MAKE_THIS_WOP

    were::emit(this_wop, &were_object::destroyed);
    collapsed_ = true;
}
