#include "sparkle_main_activity.h"
#include <jni.h>
#include <thread>

extern "C"
{
#include <lauxlib.h>
#include <lualib.h>
#include <lua.h>
}

#include "sparkle_android_logger.h"

void lua_thread()
{
    sparkle_android_logger logger;

    int status;

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    status = luaL_loadfile(L, "/data/data/com.sion.sparkle/user.lua");
    if (status)
    {
        fprintf(stderr, "Failed to load script: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return;
    }

    status = lua_pcall(L, 0, 0, 0);
    if (status)
    {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return;
    }

    lua_getglobal(L, "start");

    status = lua_pcall(L, 0, 0, 0);
    if (status)
    {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return;
    }

    lua_close(L);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_MainActivity_native_1start(JNIEnv *env, jobject instance)
{
    std::thread lua(lua_thread);
    lua.detach();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_MainActivity_native_1stop(JNIEnv *env, jobject instance)
{
}
