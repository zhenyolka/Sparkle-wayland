#include "sparkle_java_object.h"
#include "were_exception.h"

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
    jclass class__ = env->GetObjectClass(instance);
    if (class__ == nullptr)
        throw were_exception(WE_SIMPLE);

    class_ = (jclass)env->NewGlobalRef(class__);

    object_ = env->NewGlobalRef(instance);
}

sparkle_java_object::sparkle_java_object(JNIEnv *env, const char *class_id, const char *constructor_id, ...)
{
    jclass class__ = env->FindClass(class_id);
    if (class__ == nullptr)
        throw were_exception(WE_SIMPLE);

    class_ = (jclass)env->NewGlobalRef(class__);

    va_list ap;
    va_start(ap, constructor_id);
    jobject instance = env->NewObjectV(class1(), get_method_id("<init>", constructor_id), ap);
    va_end(ap);

    object_ = env->NewGlobalRef(instance);
}

JNIEnv *sparkle_java_object::env()
{
    JNIEnv *env;

    jint res = javaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (res != JNI_OK)
        throw were_exception(WE_SIMPLE);

    return env;
}

jmethodID sparkle_java_object::get_method_id(const char *name, const char *signature)
{
    jmethodID id = env()->GetMethodID(class1(), name, signature);
    if (id == nullptr)
    {
        fprintf(stderr, "%s\n", name);
        throw were_exception(WE_SIMPLE);
    }

    return id;
}

void sparkle_java_object::call_void_method(const char *name, const char *signature, ...)
{
    va_list ap;
    va_start(ap, signature);
    env()->CallVoidMethodV(object1(), get_method_id(name, signature), ap);
    va_end(ap);
}

int sparkle_java_object::call_int_method(const char *name, const char *signature, ...)
{
    va_list ap;
    va_start(ap, signature);
    int r = env()->CallIntMethodV(object1(), get_method_id(name, signature), ap);
    va_end(ap);

    return r;
}

std::string sparkle_java_object::call_string_method(const char *name, const char *signature, ...)
{
    va_list ap;
    va_start(ap, signature);
    jobject java_string__ = env()->CallObjectMethodV(object1(), get_method_id(name, signature), ap);
    jstring java_string = reinterpret_cast<jstring>(java_string__);
    va_end(ap);

    const char *chars = env()->GetStringUTFChars(java_string, NULL);
    if (!chars)
        throw were_exception(WE_SIMPLE);

    std::string r(chars);

    env()->ReleaseStringUTFChars(java_string, chars);

    env()->DeleteLocalRef(java_string);

    return r;
}
