#ifndef SPARKLE_JAVA_OBJECT_H
#define SPARKLE_JAVA_OBJECT_H

#include <jni.h>

extern JavaVM *javaVM;

class sparkle_java_object
{
public:
    ~sparkle_java_object();
    sparkle_java_object(JNIEnv *env, jobject instance);

    template <typename ...Args>
    sparkle_java_object(JNIEnv *env, const char *class_id, const char *constructor_id, Args... args)
    {
            jclass class__ = env->FindClass(class_id); // XXX More checks
            jmethodID id = env->GetMethodID(class__, "<init>", constructor_id);
            jobject instance = env->NewObject(class__, id, args...);

            class_ = (jclass)env->NewGlobalRef(class__);
            object_ = env->NewGlobalRef(instance);
    }

    static JNIEnv *env();
    jclass class1() const {return class_;}
    jobject object1() const {return object_;}

private:
    jclass class_;
    jobject object_;
};

#endif // SPARKLE_JAVA_OBJECT_H
