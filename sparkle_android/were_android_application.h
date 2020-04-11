#ifndef WERE_ANDROID_APPLICATION_H
#define WERE_ANDROID_APPLICATION_H

#include "were.h"
#include "sparkle_java_object.h"

class AAssetManager;

class were_android_application : virtual public were_object, public sparkle_java_object
{
public:
    ~were_android_application();
    were_android_application(JNIEnv *env, jobject instance);

    void access() const override {}

    std::string files_dir() const { return files_dir_; }
    std::string home_dir() const { return home_dir_; }

    void enable_native_loop(int fd);
    void disable_native_loop();

private:
    void copy_asset(AAssetManager *assets, const char *source, const char *destination, mode_t mode);
    void setup();

private:
    std::string files_dir_;
    std::string home_dir_;
};

#endif // WERE_ANDROID_APPLICATION_H
