#include "sparkle_java_object.h"
#include <exception>

JavaVM *javaVM;

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    javaVM = vm;

    JNIEnv *env;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;

    return JNI_VERSION_1_6;
}

sparkle_java_object::~sparkle_java_object()
{
    env()->DeleteGlobalRef(class_);
    env()->DeleteGlobalRef(object_);
}

sparkle_java_object::sparkle_java_object(JNIEnv *env, jobject instance)
{
    jclass class__ = env->GetObjectClass(instance); // XXX More checks

    class_ = (jclass)env->NewGlobalRef(class__);
    object_ = env->NewGlobalRef(instance);
}

JNIEnv *sparkle_java_object::env()
{
    JNIEnv *env;

    jint res = javaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (res != JNI_OK)
        throw std::exception();

    return env;
}
