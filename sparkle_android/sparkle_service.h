#ifndef SPARKLE_SERVICE_H
#define SPARKLE_SERVICE_H

#include "were_object.h"
#include "sparkle_java_object.h"

class were_surface_producer;
class sparkle_surface;
class sparkle_keyboard;
class sparkle_pointer;
class sparkle_touch;

class sparkle_service : public were_object, public sparkle_java_object
{
public:
    ~sparkle_service();
    sparkle_service(JNIEnv *env, jobject instance);

    int display_width() const;
    int display_height() const;

    std::string files_dir() const { return files_dir_; }

    void enable_native_loop(int fd);
    void disable_native_loop();

signals:
    were_signal<void (were_object_pointer<sparkle_surface> surface)> surface_created;
    were_signal<void (were_object_pointer<sparkle_keyboard> keyboard)> keyboard_created;
    were_signal<void (were_object_pointer<sparkle_pointer> pointer)> pointer_created;
    were_signal<void (were_object_pointer<sparkle_touch> touch)> touch_created;

private:
    void register_producer(were_object_pointer<were_surface_producer> producer);

private:
    std::string files_dir_;
};

#endif // SPARKLE_SERVICE_H
