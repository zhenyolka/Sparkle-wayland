#include "SparkleJavaObject.h"
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

SparkleJavaObject::~SparkleJavaObject()
{
    env()->DeleteGlobalRef(class__);
    env()->DeleteGlobalRef(object__);
}

SparkleJavaObject::SparkleJavaObject(JNIEnv *env, jobject instance)
{
    jclass clz = env->GetObjectClass(instance);
    class__ = (jclass)env->NewGlobalRef(clz);
    object__ = env->NewGlobalRef(instance);
}

JNIEnv *SparkleJavaObject::env()
{
    JNIEnv *env;

    jint res = javaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (res != JNI_OK)
        throw std::exception();

    return env;
}
