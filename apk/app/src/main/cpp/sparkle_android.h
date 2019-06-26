#ifndef SPARKLE_ANDROID_H
#define SPARKLE_ANDROID_H

#include "sparkle.h"

class sparkle_android : public were_object_2
{
public:
    ~sparkle_android();
    sparkle_android(were_object_pointer<sparkle> sparkle);

signals:
    //were_signal<void (were_object_pointer<sparkle_android_surface> android_surface)> android_surface_created;
    //were_signal<void (were_object_pointer<sparkle_keyboard> keyboard)> keyboard_created;
    //were_signal<void (were_object_pointer<sparkle_pointer> pointer)> pointer_created;

private:
    //static void connect_keyboard(were_object_pointer<sparkle_x11_surface> x11_surface, were_object_pointer<sparkle_keyboard> keyboard);
    //static void connect_pointer(were_object_pointer<sparkle_x11_surface> x11_surface, were_object_pointer<sparkle_pointer> pointer);


};

#endif // SPARKLE_ANDROID_H
