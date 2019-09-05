#ifndef SPARKLE_SERVICE_H
#define SPARKLE_SERVICE_H

#include "sparkle_java_object.h"


class sparkle_service : public sparkle_java_object
{
public:
    ~sparkle_service();
    sparkle_service(JNIEnv *env, jobject instance);

    std::string files_dir() const {return files_dir_;}

    void enable_native_loop(int fd);
    void disable_native_loop();

    int display_width();
    int display_height();

private:
    std::string files_dir_;
};

#endif // SPARKLE_SERVICE_H
