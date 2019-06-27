#include "were_object.h"
#include <set>
#include <mutex>
#include <cstdio>
#include <typeinfo>

int object_count_ = 0;

std::set<were_object *> object_set_;
std::mutex object_set_mutex_;

const char *state_normal = "NORMAL";
const char *state_collapsed = "COLLAPSED";
const char *state_lost = "LOST";

void were_debug_add_object(were_object *object__)
{
    object_set_mutex_.lock();
    object_set_.insert(object__);
    object_set_mutex_.unlock();
    object_count_ += 1;
}

void were_debug_remove_object(were_object *object__)
{
    object_set_mutex_.lock();
    object_set_.erase(object__);
    object_set_mutex_.unlock();
    object_count_ -= 1;
}

int were_debug_object_count()
{
    return object_count_;
}

void were_debug_print_objects()
{
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
}
