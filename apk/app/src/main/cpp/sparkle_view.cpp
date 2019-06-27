#include "sparkle_view.h"
#include "sion_keymap.h"
#include <android/native_window_jni.h>
#include <linux/input-event-codes.h>

static const int button_map[5] = {0, BTN_LEFT, BTN_RIGHT, 0, BTN_MIDDLE};

sparkle_view::~sparkle_view()
{
}

sparkle_view::sparkle_view(JNIEnv *env, were_object_pointer<sparkle_service> service) :
    sparkle_java_object(env, "com/sion/sparkle/SparkleView", "(Lcom/sion/sparkle/SparkleService;J)V", service->object1(), jlong(this))
{
}

void sparkle_view::set_enabled(bool enabled)
{
    jmethodID id = env()->GetMethodID(class1(), "set_enabled", "(Z)V");
    env()->CallVoidMethod(object1(), id, jboolean(enabled));
}

void sparkle_view::set_visible(bool visible)
{
    jmethodID id = env()->GetMethodID(class1(), "set_visible", "(Z)V");
    env()->CallVoidMethod(object1(), id, jboolean(visible));
}

void sparkle_view::set_position(int x, int y)
{
    jmethodID id = env()->GetMethodID(class1(), "set_position", "(II)V");
    env()->CallVoidMethod(object1(), id, jint(x), jint(y));
}

void sparkle_view::set_size(int width, int height)
{
    jmethodID id = env()->GetMethodID(class1(), "set_size", "(II)V");
    env()->CallVoidMethod(object1(), id, jint(width), jint(height));
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_surface_1changed(JNIEnv *env, jobject instance, jlong user, jobject surface)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));

    ANativeWindow *window;

    if (surface)
        window = ANativeWindow_fromSurface(env, surface);
    else
        window = nullptr;

    were::emit(view, &sparkle_view::surface_changed, window);
}

/* Keyboard */

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_key_1down(JNIEnv *env, jobject instance, jlong user, jint code)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    int x = sion_keymap[code];
    if (x != 0)
        were::emit(view, &sparkle_view::key_down, x);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_key_1up(JNIEnv *env, jobject instance, jlong user, jint code)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    int x = sion_keymap[code];
    if (x != 0)
        were::emit(view, &sparkle_view::key_up, x);
}

/* Pointer */

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1button_1down(JNIEnv *env, jobject instance, jlong user, jint button)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view, &sparkle_view::pointer_button_down, button_map[button]);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1button_1up(JNIEnv *env, jobject instance, jlong user, jint button)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view, &sparkle_view::pointer_button_up, button_map[button]);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1motion(JNIEnv *env, jobject instance, jlong user, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view, &sparkle_view::pointer_motion, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1enter(JNIEnv *env, jobject instance, jlong user)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view, &sparkle_view::pointer_enter);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1leave(JNIEnv *env, jobject instance, jlong user)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view, &sparkle_view::pointer_leave);
}

/* Touch */

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1down(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view, &sparkle_view::touch_down, id, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1up(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view, &sparkle_view::touch_up, id, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1motion(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view, &sparkle_view::touch_motion, id, x, y);
}
