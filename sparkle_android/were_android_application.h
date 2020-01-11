#ifndef WERE_ANDROID_APPLICATION_H
#define WERE_ANDROID_APPLICATION_H

#include "were_object.h"
#include "sparkle_java_object.h"

class were_android_application : public were_object, public sparkle_java_object
{
public:
    ~were_android_application();
    were_android_application(JNIEnv *env, jobject instance);

    std::string files_dir() const { return files_dir_; }
    std::string home_dir() const { return home_dir_; }

    void enable_native_loop(int fd);
    void disable_native_loop();

private:
    std::string files_dir_;
    std::string home_dir_;
};

#endif // WERE_ANDROID_APPLICATION_H
