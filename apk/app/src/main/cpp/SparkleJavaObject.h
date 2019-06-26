#ifndef SPARKLE_JAVA_OBJECT_H
#define SPARKLE_JAVA_OBJECT_H

#include <jni.h>

extern JavaVM *javaVM;

class SparkleJavaObject
{
public:
    ~SparkleJavaObject();
    SparkleJavaObject(JNIEnv *env, jobject instance);

    static JNIEnv *env();
    jclass class_() const {return class__;}
    jobject object_() const {return object__;}

private:
    jclass class__;
    jobject object__;
};

#endif // SPARKLE_JAVA_OBJECT_H
