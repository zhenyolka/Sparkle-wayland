#include "sparkle_main_activity.h"
#include <jni.h>
#include <thread>

#include "were_thread.h"
#include "were_registry.h"
#include "sparkle_settings.h"

//#include <sys/stat.h> // chmod()
#include <unistd.h> // chdir()
#include <cstdlib> // system()

#include "were_android_application.h"


sparkle_main_activity::~sparkle_main_activity()
{
    fprintf(stdout, "~sparkle_main_activity\n");

    if (user_thread_.joinable())
        user_thread_.join();
}

sparkle_main_activity::sparkle_main_activity(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance), user_busy_(false)
{
    fprintf(stdout, "sparkle_main_activity\n");
}

void sparkle_main_activity::user()
{
    chdir(global<were_android_application>()->files_dir().c_str());

    std::string command = global<sparkle_settings>()->get<std::string>("on_start", "");

    std::system(command.c_str());

    user_busy_ = false;
}

void sparkle_main_activity::start()
{
    global<sparkle_settings>()->load(); // XXX2 Auto reload based on timestamp?

    if (!user_busy_)
    {
        user_busy_ = true;

        if (user_thread_.joinable())
            user_thread_.join();

        user_thread_ = std::thread(&sparkle_main_activity::user, this);
    }
}

void sparkle_main_activity::stop()
{
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_sion_sparkle_MainActivity_native_1create(JNIEnv *env, jobject instance)
{
    were_object_pointer<sparkle_main_activity> native__(new sparkle_main_activity(env, instance));
    native__.increment_reference_count();
    return jlong(native__.access());
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_MainActivity_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    were_object_pointer<sparkle_main_activity> native__(reinterpret_cast<sparkle_main_activity *>(native));
    native__.decrement_reference_count();
    native__.collapse();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_MainActivity_native_1start(JNIEnv *env, jobject instance, jlong native)
{
    were_object_pointer<sparkle_main_activity> native__(reinterpret_cast<sparkle_main_activity *>(native));
    native__->start();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_MainActivity_native_1stop(JNIEnv *env, jobject instance, jlong native)
{
    were_object_pointer<sparkle_main_activity> native__(reinterpret_cast<sparkle_main_activity *>(native));
    native__->stop();
}
