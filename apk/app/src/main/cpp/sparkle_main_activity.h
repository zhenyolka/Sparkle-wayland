#ifndef SPARKLE_MAIN_ACTIVITY_H
#define SPARKLE_MAIN_ACTIVITY_H

#include "sparkle_java_object.h"
#include "sparkle_android_logger.h" // XXX2
#include <thread>

class sparkle_main_activity : public sparkle_java_object
{
public:
    ~sparkle_main_activity();
    sparkle_main_activity(JNIEnv *env, jobject instance);

    void start();
    void stop();

private:
    void lua();

private:
    sparkle_android_logger logger_;
    std::thread lua_thread_;
    bool lua_done_;
};

#endif // SPARKLE_MAIN_ACTIVITY_H
