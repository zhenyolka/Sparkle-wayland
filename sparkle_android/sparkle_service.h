#ifndef SPARKLE_SERVICE_H
#define SPARKLE_SERVICE_H

#include "were_object.h"
#include "sparkle_java_object.h"

class were_surface_producer;
class sparkle;

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

private:
    void register_producer(were_object_pointer<were_surface_producer> producer);

private:
    std::string files_dir_;
    were_object_pointer<sparkle> sparkle_;
};

#endif // SPARKLE_SERVICE_H
