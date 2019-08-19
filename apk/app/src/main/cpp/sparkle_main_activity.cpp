#include "sparkle_main_activity.h"
#include <jni.h>
#include <thread>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "sparkle_android_logger.h"
#include "were_backtrace.h"

extern "C"
{
#include <lauxlib.h>
#include <lualib.h>
#include <lua.h>
}

#include <sys/stat.h> // chmod
#include <unistd.h> // chdir



sparkle_main_activity::~sparkle_main_activity()
{
    if (lua_thread_.joinable())
        lua_thread_.join();
}

sparkle_main_activity::sparkle_main_activity(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance), lua_done_(true)
{
    files_dir_ = call_string_method("files_dir", "()Ljava/lang/String;");
    home_dir_ = call_string_method("home_dir", "()Ljava/lang/String;");
}

void sparkle_main_activity::lua()
{
    int status;

    if (chdir(files_dir_.c_str()) == -1)
        throw were_exception(WE_SIMPLE);

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    status = luaL_loadfile(L, "user.lua");
    if (status)
    {
        fprintf(stderr, "Failed to load script: %s\n", lua_tostring(L, -1));
        goto finish;
    }

    status = lua_pcall(L, 0, 0, 0);
    if (status)
    {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        goto finish;
    }

    lua_getglobal(L, "start");

    status = lua_pcall(L, 0, 0, 0);
    if (status)
    {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        goto finish;
    }

finish:
    lua_close(L);
    lua_done_ = true;
}

void sparkle_main_activity::copy_asset(AAssetManager *assets, const char *source, const char *destination, mode_t mode)
{
    std::string out__ = files_dir_ + "/" + destination;

    if(access(out__.c_str(), F_OK) != -1)
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
    if (chmod(home_dir_.c_str(), 0755) == -1)
        throw were_exception(WE_SIMPLE);
    if (chmod(files_dir_.c_str(), 0755) == -1)
        throw were_exception(WE_SIMPLE);


    jobject java_assets = call_object_method("getAssets", "()Landroid/content/res/AssetManager;");

    AAssetManager *assets = AAssetManager_fromJava(env(), java_assets);
    if (assets == nullptr)
        throw were_exception(WE_SIMPLE);

    copy_asset(assets, "settings.lua", "settings.lua", 0644);
    copy_asset(assets, "sparkle.lua", "sparkle.lua", 0644);
    copy_asset(assets, "user.lua", "user.lua", 0644);
    copy_asset(assets, "sparkle.sh", "sparkle.sh", 0644);
    copy_asset(assets, "user.sh", "user.sh", 0644);

    env()->DeleteLocalRef(java_assets);
}

void sparkle_main_activity::start()
{
    if (lua_done_)
    {
        lua_done_ = false;

        if (lua_thread_.joinable())
            lua_thread_.join();

        lua_thread_ = std::thread(&sparkle_main_activity::lua, this);
    }
}

void sparkle_main_activity::stop()
{
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_sion_sparkle_MainActivity_native_1create(JNIEnv *env, jobject instance)
{
    were_backtrace::instance().enable();

    were_object_pointer<sparkle_main_activity> native__(new sparkle_main_activity(env, instance));
    native__->increment_reference_count();
    sparkle_android_logger::instance().redirect_output(native__->files_dir() + "/log.txt");

    return jlong(native__.access());
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_MainActivity_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    were_object_pointer<sparkle_main_activity> native__(reinterpret_cast<sparkle_main_activity *>(native));
    native__->decrement_reference_count();
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
