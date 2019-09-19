#ifndef SPARKLE_PLATFORM_H
#define SPARKLE_PLATFORM_H

#include "were_object.h"

class sparkle;
class were_platform_surface_provider;
class sparkle_keyboard;
class sparkle_pointer;
class sparkle_touch;


class sparkle_platform_surface;

class sparkle_platform : public were_object
{
public:
    ~sparkle_platform();
    sparkle_platform(were_object_pointer<sparkle> sparkle, were_object_pointer<were_platform_surface_provider> platform_surface_provider);

    were_object_pointer<sparkle> sparkle1() const { return sparkle_; }
    were_object_pointer<were_platform_surface_provider> platform_surface_provider() const { return platform_surface_provider_; }

signals:
    were_signal<void (were_object_pointer<sparkle_platform_surface> platform_surface)> platform_surface_created;
    were_signal<void (were_object_pointer<sparkle_keyboard> keyboard)> keyboard_created;
    were_signal<void (were_object_pointer<sparkle_pointer> pointer)> pointer_created;
    were_signal<void (were_object_pointer<sparkle_touch> touch)> touch_created;

private:
    were_object_pointer<sparkle> sparkle_;
    were_object_pointer<were_platform_surface_provider> platform_surface_provider_;
};

#endif // SPARKLE_PLATFORM_H
