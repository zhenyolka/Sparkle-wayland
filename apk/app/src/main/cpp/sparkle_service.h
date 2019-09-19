#ifndef SPARKLE_SERVICE_H
#define SPARKLE_SERVICE_H

#include "were_platform_surface_provider.h"
#include "sparkle_java_object.h"

class were_platform_surface;

class sparkle_service : public were_platform_surface_provider, public sparkle_java_object
{
public:
    ~sparkle_service();
    sparkle_service(JNIEnv *env, jobject instance);

    int display_width() const;
    int display_height() const;
    were_object_pointer<were_platform_surface> create_surface();

    std::string files_dir() const { return files_dir_; }

    void enable_native_loop(int fd);
    void disable_native_loop();

    int display_width();
    int display_height();

private:
    std::string files_dir_;
};

#endif // SPARKLE_SERVICE_H
