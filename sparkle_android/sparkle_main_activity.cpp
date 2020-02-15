#include "sparkle_main_activity.h"
#include <jni.h>
#include <thread>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "were_thread.h"
#include "were_registry.h"

#include <sys/stat.h> // chmod()
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
    std::string command;
    command += "/system/bin/sh ";
    command += t_l_global<were_android_application>()->files_dir() + "/user.sh";

    std::system(command.c_str());

    user_busy_ = false;
}

void sparkle_main_activity::copy_asset(AAssetManager *assets, const char *source, const char *destination, mode_t mode)
{
    std::string out__ = t_l_global<were_android_application>()->files_dir() + "/" + destination;

    if(::access(out__.c_str(), F_OK) != -1)
        return;

    AAsset *asset = AAssetManager_open(assets, source, AASSET_MODE_STREAMING);
    if (asset == nullptr)
        throw were_exception(WE_SIMPLE);

    char buffer[BUFSIZ];
    int n = 0;

    FILE *out = fopen(out__.c_str(), "w");
    if (out == nullptr)
        throw were_exception(WE_SIMPLE);

    while ((n = AAsset_read(asset, buffer, BUFSIZ)) > 0)
        fwrite(buffer, n, 1, out);

    fclose(out);

    AAsset_close(asset);

    if (chmod(out__.c_str(), mode) == -1)
        throw were_exception(WE_SIMPLE);
}

void sparkle_main_activity::setup()
{
    if (chmod(t_l_global<were_android_application>()->home_dir().c_str(), 0755) == -1)
        throw were_exception(WE_SIMPLE);
    if (chmod(t_l_global<were_android_application>()->files_dir().c_str(), 0755) == -1)
        throw were_exception(WE_SIMPLE);


    jobject java_assets = call_object_method("getAssets", "()Landroid/content/res/AssetManager;");

    AAssetManager *assets = AAssetManager_fromJava(env(), java_assets);
    if (assets == nullptr)
        throw were_exception(WE_SIMPLE);

    copy_asset(assets, "sparkle.config", "sparkle.config", 0644);
    copy_asset(assets, "sparkle.sh", "sparkle.sh", 0644);
    copy_asset(assets, "user.sh", "user.sh", 0644);

    env()->DeleteLocalRef(java_assets);
}

void sparkle_main_activity::start()
{
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
Java_com_sion_sparkle_MainActivity_native_1setup(JNIEnv *env, jobject instance, jlong native)
{
    were_object_pointer<sparkle_main_activity> native__(reinterpret_cast<sparkle_main_activity *>(native));
    native__->setup();
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
