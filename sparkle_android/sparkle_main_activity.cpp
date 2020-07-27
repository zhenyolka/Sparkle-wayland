#include "sparkle_main_activity.h"
#include "were_android_application.h"
#include "sparkle_settings.h"
#include <jni.h>
#include <thread>
//#include <sys/stat.h> // chmod()
#include <unistd.h> // chdir()
#include <cstdlib> // system()



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
    chdir(were_slot<were_pointer<were_android_application>>::get()->files_dir().c_str());

    std::string command = were_slot<were_pointer<sparkle_settings>>::get()->get<std::string>("on_start", "");

    std::system(command.c_str());

    user_busy_ = false;
}

void sparkle_main_activity::start()
{
    were_slot<were_pointer<sparkle_settings>>::get()->load(); // XXX2 Auto reload based on timestamp?

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
    were_pointer<sparkle_main_activity> native__ = were_new<sparkle_main_activity>(env, instance);
    native__.increment_reference_count();
    return jlong(native__.access());
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_MainActivity_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    were_pointer<sparkle_main_activity> native__(reinterpret_cast<sparkle_main_activity *>(native));
    native__.decrement_reference_count();
    native__.collapse();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_MainActivity_native_1start(JNIEnv *env, jobject instance, jlong native)
{
    were_pointer<sparkle_main_activity> native__(reinterpret_cast<sparkle_main_activity *>(native));
    native__->start();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_MainActivity_native_1stop(JNIEnv *env, jobject instance, jlong native)
{
    were_pointer<sparkle_main_activity> native__(reinterpret_cast<sparkle_main_activity *>(native));
    native__->stop();
}
