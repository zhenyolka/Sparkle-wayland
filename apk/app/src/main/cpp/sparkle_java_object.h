#ifndef SPARKLE_JAVA_OBJECT_H
#define SPARKLE_JAVA_OBJECT_H

#include "were_object_2.h"
#include <jni.h>
#include <string>

extern JavaVM *javaVM;

class sparkle_java_object : public were_object_2
{
public:
    ~sparkle_java_object();
    sparkle_java_object(JNIEnv *env, jobject instance);
    sparkle_java_object(JNIEnv *env, const char *class_id, const char *constructor_id, ...);

    static JNIEnv *env();
    jclass class1() const {return class_;}
    jobject object1() const {return object_;}

    jmethodID get_method_id(const char *name, const char *signature);
    void call_void_method(const char *name, const char *signature, ...);
    int call_int_method(const char *name, const char *signature, ...);
    std::string call_string_method(const char *name, const char *signature, ...);
    jobject call_object_method(const char *name, const char *signature, ...);

private:
    jclass class_;
    jobject object_;
};

#endif // SPARKLE_JAVA_OBJECT_H
