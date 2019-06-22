#ifndef SPARKLE_GLOBAL_H
#define SPARKLE_GLOBAL_H

#include "were_object_2.h"
#include "wayland-server.h"


template <typename T>
class sparkle_global : public were_object_2
{
public:

    ~sparkle_global()
    {
        wl_global_destroy(global_);
    }

    sparkle_global(struct wl_display *display, const struct wl_interface *interface, int version)
    {
        global_ = wl_global_create(display, interface, version, this, sparkle_global::bind_);
        if (global_ == nullptr)
            throw were_exception(WE_SIMPLE);
    }

signals:
    were_signal<void (were_object_pointer<T> instance)> instance;

private:
    static void bind_(struct wl_client *client, void *data, uint32_t version, uint32_t id)
    {
        sparkle_global *instance = reinterpret_cast<sparkle_global *>(data);
        were_object_pointer<sparkle_global> instance__(instance);

        were_object_pointer<T> object__(new T(client, version, id));

        were::emit(instance__, &sparkle_global::instance, object__);
    }

private:
    struct wl_global *global_;
};

#endif // SPARKLE_GLOBAL_H
