#ifndef SPARKLE_ANDROID_H
#define SPARKLE_ANDROID_H

#include "sparkle.h"

class sparkle_service;
class sparkle_android_surface;
class sparkle_keyboard;
class sparkle_pointer;
class sparkle_touch;

class sparkle_android : public were_object_2
{
public:
    ~sparkle_android();
    sparkle_android(were_object_pointer<sparkle> sparkle, were_object_pointer<sparkle_service> service);

    were_object_pointer<sparkle_service> service() const {return service_;}

signals:
    were_signal<void (were_object_pointer<sparkle_android_surface> android_surface)> android_surface_created;
    were_signal<void (were_object_pointer<sparkle_keyboard> keyboard)> keyboard_created;
    were_signal<void (were_object_pointer<sparkle_pointer> pointer)> pointer_created;
    were_signal<void (were_object_pointer<sparkle_touch> touch)> touch_created;

private:
    were_object_pointer<sparkle_service> service_;
};

#endif // SPARKLE_ANDROID_H
