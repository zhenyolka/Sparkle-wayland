#include "sparkle_resource.h"
#include "were_exception.h"

#include <cstdio>

sparkle_resource::~sparkle_resource()
{
    wl_list_remove(&listener_.link);

    // XXX
}

sparkle_resource::sparkle_resource(struct wl_client *client, const struct wl_interface *interface, int version, uint32_t id, const void *implementation)
{
    resource_ = wl_resource_create(client, interface, version, id);
    if (resource_ == nullptr)
        throw were_exception(WE_SIMPLE);

    wl_resource_set_implementation(resource_, implementation, this, nullptr);

    listener_.notify = sparkle_resource::destroy_;
    wl_resource_add_destroy_listener(resource_, &listener_);
}

void sparkle_resource::destroy_(struct wl_listener *listener, void *data)
{
    sparkle_resource *instance;
    instance = wl_container_of(listener, instance, listener_); // XXX

    instance->resource_ = nullptr;
    were_object_pointer<sparkle_resource> instance__(instance);
    instance__.collapse(); // XXX Signal?
}

struct wl_resource *sparkle_resource::resource() const
{
    if (resource_ == nullptr)
        throw were_exception(WE_SIMPLE);

    return resource_;
}

int sparkle_resource::version() const
{
    return wl_resource_get_version(resource());
}

struct wl_client *sparkle_resource::client() const
{
    return wl_resource_get_client(resource());
}