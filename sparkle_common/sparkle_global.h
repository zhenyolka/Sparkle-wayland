#ifndef SPARKLE_GLOBAL_H
#define SPARKLE_GLOBAL_H

#include "sparkle.h"
#include "wayland-server.h"
#include "were_object_wrapper.h"


template <typename T>
class sparkle_global : virtual public were_object
{
public:

    ~sparkle_global()
    {
        wl_global_destroy(global_);
    }

    sparkle_global(were_pointer<sparkle_display> display, const struct wl_interface *interface, int version) :
        display_(display)
    {
        global_ = wl_global_create(display_->get(), interface, version, this, sparkle_global::bind_);
        if (global_ == nullptr)
            throw were_exception(WE_SIMPLE);
    }

signals:
    were_signal<void (were_pointer<T> instance)> instance;

private:
    static void bind_(struct wl_client *client, void *data, uint32_t version, uint32_t id)
    {
        sparkle_global *instance = reinterpret_cast<sparkle_global *>(data);
        were_pointer<sparkle_global> instance__(instance);

        were_pointer<T> object__ = were_new<T>(client, version, id, instance__->display_);

        were::emit(instance__, &sparkle_global::instance, object__);
    }

private:
    were_pointer<sparkle_display> display_;
    struct wl_global *global_;
};

#endif // SPARKLE_GLOBAL_H
