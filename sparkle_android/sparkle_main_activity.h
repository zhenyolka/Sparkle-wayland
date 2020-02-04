#ifndef SPARKLE_MAIN_ACTIVITY_H
#define SPARKLE_MAIN_ACTIVITY_H

#include "were_object.h"
#include "sparkle_java_object.h"
#include <thread>

class AAssetManager;

class sparkle_main_activity : public were_object, public sparkle_java_object
{
public:
    ~sparkle_main_activity();
    sparkle_main_activity(JNIEnv *env, jobject instance);

    void setup();
    void start();
    void stop();

private:
    void copy_asset(AAssetManager *assets, const char *source, const char *destination, mode_t mode);
    void user();

private:
    std::thread user_thread_;
    bool user_busy_;
};

#endif // SPARKLE_MAIN_ACTIVITY_H
