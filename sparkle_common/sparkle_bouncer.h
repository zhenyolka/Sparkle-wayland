#ifndef SPARKLE_BOUNCER_H
#define SPARKLE_BOUNCER_H

#include "were_object_2.h"
#include <wayland-server.h>

#include <cstdio>

/*
template<class T, class Method, Method m, class ...Params>
static auto bounce(void *priv, Params... params) ->
    decltype(((*reinterpret_cast<T *>(priv)).*m)(params...))
{
    return ((*reinterpret_cast<T *>(priv)).*m)(params...);
}
#define BOUNCE(c, m) bounce<c, decltype(&c::m), &c::m>
//register_callback(&bounce<FooCB, decltype(&FooCB::callback), &FooCB::callback>, &my_foo);
//register_callback(&BOUNCE(FooCB, callback), &my_foo);
*/

class sparkle_bouncer
{
public:

    template<class T, class Method, Method method__, class ...Args>
    static void bouncer_1(struct wl_client *client, struct wl_resource *resource, Args... args)
    {
        T *object__ = reinterpret_cast<T *>(wl_resource_get_user_data(resource));
        ((*object__).*method__)(client, resource, args...);
    }

    template<class T, class Method, Method method__, class ...Args>
    static void bouncer_2(struct wl_client *client, struct wl_resource *resource, Args... args)
    {
        T *object__ = reinterpret_cast<T *>(wl_resource_get_user_data(resource));
        were_object_pointer<T> object___(object__);
        were::emit(object___, method__, args...);
    }

    template<class T, class Method, Method method__, class ...Args>
    static void bouncer_2_d(struct wl_client *client, struct wl_resource *resource, Args... args)
    {
        T *object__ = reinterpret_cast<T *>(wl_resource_get_user_data(resource));
        were_object_pointer<T> object___(object__);
        were::emit(object___, method__, args...);

        wl_resource_destroy(resource); // XXX
    }
};

#define BOUNCER_1(c, m) sparkle_bouncer::bouncer_1<c, decltype(&c::m), &c::m>
#define BOUNCER_2(c, m) sparkle_bouncer::bouncer_2<c, decltype(&c::m), &c::m>
#define BOUNCER_2_D(c, m) sparkle_bouncer::bouncer_2_d<c, decltype(&c::m), &c::m>

#endif // SPARKLE_BOUNCER_H
