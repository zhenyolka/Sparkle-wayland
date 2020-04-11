#ifndef SPARKLE_MAIN_ACTIVITY_H
#define SPARKLE_MAIN_ACTIVITY_H

#include "were.h"
#include "sparkle_java_object.h"
#include <thread>

class sparkle_main_activity : virtual public were_object, public sparkle_java_object
{
public:
    ~sparkle_main_activity();
    sparkle_main_activity(JNIEnv *env, jobject instance);

    void start();
    void stop();

private:
    void user();

private:
    std::thread user_thread_;
    bool user_busy_;
};

#endif // SPARKLE_MAIN_ACTIVITY_H
