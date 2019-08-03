#include "sparkle_main_activity.h"
#include <jni.h>
#include <thread>

extern "C"
{
#include <lauxlib.h>
#include <lualib.h>
#include <lua.h>
}

#include <unistd.h> // chdir


sparkle_main_activity::~sparkle_main_activity()
{
    if (lua_thread_.joinable())
        lua_thread_.join();
}

sparkle_main_activity::sparkle_main_activity(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance), lua_done_(true)
{
}

void sparkle_main_activity::lua()
{
    int status;

    chdir("/data/data/com.sion.sparkle/");

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    status = luaL_loadfile(L, "/data/data/com.sion.sparkle/user.lua");
    if (status)
    {
        fprintf(stderr, "Failed to load script: %s\n", lua_tostring(L, -1));
        lua_close(L);
        lua_done_ = true; // XXX2
        return;
    }

    status = lua_pcall(L, 0, 0, 0);
    if (status)
    {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        lua_close(L);
        lua_done_ = true;
        return;
    }

    lua_getglobal(L, "start");

    status = lua_pcall(L, 0, 0, 0);
    if (status)
    {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        lua_close(L);
        lua_done_ = true;
        return;
    }

    lua_close(L);
    lua_done_ = true;
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
    were_object_pointer<sparkle_main_activity> native__(new sparkle_main_activity(env, instance));
    native__->increment_reference_count();

    return jlong(native__.get());
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_MainActivity_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    were_object_pointer<sparkle_main_activity> native__(reinterpret_cast<sparkle_main_activity *>(native));
    native__->decrement_reference_count();
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
