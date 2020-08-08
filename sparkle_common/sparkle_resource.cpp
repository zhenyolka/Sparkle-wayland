#include "sparkle_resource.h"
#include <cstdio>


template <typename T, typename M>
static inline constexpr ptrdiff_t offset_of(const M T::*member)
{
    return reinterpret_cast<ptrdiff_t>(&(reinterpret_cast<T *>(0)->*member));
}

template <typename T, typename M>
static inline constexpr T *owner_of(M *ptr, const M T::*member)
{
    return reinterpret_cast<T *>(reinterpret_cast<intptr_t>(ptr) - offset_of(member));
}

sparkle_resource::~sparkle_resource()
{
}

sparkle_resource::sparkle_resource(struct wl_client *client, const struct wl_interface *interface, int version, uint32_t id, const void *implementation) :
    resource_(nullptr), listener_({})
{
    resource_ = wl_resource_create(client, interface, version, id);
    if (resource_ == nullptr)
        throw were_exception(WE_SIMPLE);

    wl_resource_set_implementation(resource_, implementation, this, nullptr);

    add_integrator([this]()
    {
        auto this_wop = were_pointer(this);

        listener_.notify = sparkle_resource::destroy_;

        this_wop.increment_reference_count();
        wl_resource_add_destroy_listener(resource_, &listener_);

        were::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
        {
            if (this_wop->resource_ != nullptr)
            {
                wl_list_remove(&this_wop->listener_.link);
                this_wop.decrement_reference_count();
            }
        });
    });
}

sparkle_resource::sparkle_resource(struct wl_resource *resource) :
    resource_(nullptr), listener_({})
{
    resource_ = resource;

    add_integrator([this]()
    {
        auto this_wop = were_pointer(this);

        listener_.notify = sparkle_resource::destroy_;

        this_wop.increment_reference_count();
        wl_resource_add_destroy_listener(resource_, &listener_);

        were::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
        {
            if (this_wop->resource_ != nullptr)
            {
                wl_list_remove(&this_wop->listener_.link);
                this_wop.decrement_reference_count();
            }
        });
    });
}

void sparkle_resource::destroy_(struct wl_listener *listener, void *data)
{
    sparkle_resource *instance = owner_of(listener, &sparkle_resource::listener_);

    instance->resource_ = nullptr;

    were_pointer<sparkle_resource> instance__(instance);
    instance__.decrement_reference_count();
    instance__.collapse();
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
