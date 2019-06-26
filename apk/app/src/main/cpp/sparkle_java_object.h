#ifndef SPARKLE_JAVA_OBJECT_H
#define SPARKLE_JAVA_OBJECT_H

#include <jni.h>

extern JavaVM *javaVM;

class sparkle_java_object
{
public:
    ~sparkle_java_object();
    sparkle_java_object(JNIEnv *env, jobject instance);

    static JNIEnv *env();
    jclass class1() const {return class_;}
    jobject object1() const {return object_;}

private:
    jclass class_;
    jobject object_;
};

#endif // SPARKLE_JAVA_OBJECT_H
