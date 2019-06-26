#include "SparkleView.h"
#include <android/native_window_jni.h>
#include <linux/input-event-codes.h>
#include "sion_keymap.h"

#include "SparkleLog.h"

/* ================================================================================================================== */

static const int button_map[5] = {0, BTN_LEFT, BTN_RIGHT, 0, BTN_MIDDLE};

SparkleView::~SparkleView()
{
    sparkle_debug_method(sparkle_debug_life);

    jmethodID id = env()->GetMethodID(class_(), "setUser", "(J)V");
    env()->CallVoidMethod(object_(), id, jlong(0));
}

SparkleView::SparkleView(JNIEnv *env, jobject instance) :
    SparkleJavaObject(env, instance)
{
    sparkle_debug_method(sparkle_debug_life);

    jmethodID id = env->GetMethodID(class_(), "setUser", "(J)V");
    env->CallVoidMethod(object_(), id, jlong(this));
}

void SparkleView::setVisible(bool visible)
{
    if (visible)
    {
        jmethodID id = env()->GetMethodID(class_(), "setVisible", "(Z)V");
        env()->CallVoidMethod(object_(), id, jboolean(true));
    }
    else
    {
        jmethodID id = env()->GetMethodID(class_(), "setVisible", "(Z)V");
        env()->CallVoidMethod(object_(), id, jboolean(false));
    }
}

void SparkleView::resize(int width, int height)
{
    jmethodID id = env()->GetMethodID(class_(), "resize", "(II)V");
    env()->CallVoidMethod(object_(), id, jint(width), jint(height));
}

/* ================================================================================================================== */

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_surface_1changed(JNIEnv *env, jobject instance, jlong user, jobject surface)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    view->surfaceChanged(window);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1down(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    view->touchDown(id, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1up(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    view->touchUp(id, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1motion(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    view->touchMotion(id, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_key_1press(JNIEnv *env, jobject instance, jlong user, jint code)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    int x = sion_keymap[code];
    if (x != 0)
        view->keyboardKeyPress(x - 8);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_key_1release(JNIEnv *env, jobject instance, jlong user, jint code)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    int x = sion_keymap[code];
    if (x != 0)
        view->keyboardKeyRelease(x - 8);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_mouse_1button_1press(JNIEnv *env, jobject instance, jlong user, jint button)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    view->mouseButtonPress(button_map[button]);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_mouse_1button_1release(JNIEnv *env, jobject instance, jlong user, jint button)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    view->mouseButtonRelease(button_map[button]);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_mouse_1pointer_1motion(JNIEnv *env, jobject instance, jlong user, jfloat x, jfloat y)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    view->mousePointerMotion(x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_mouse_1pointer_1enter(JNIEnv *env, jobject instance, jlong user)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    //view->mousePointerEnter();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_mouse_1pointer_1leave(JNIEnv *env, jobject instance, jlong user)
{
    SparkleView *view = reinterpret_cast<SparkleView *>(user);
    //view->mousePointerLeave();
}
