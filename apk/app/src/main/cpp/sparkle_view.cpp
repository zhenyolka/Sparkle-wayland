#include "sparkle_view.h"
#include <android/native_window_jni.h>

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
    sparkle_view *view = reinterpret_cast<sparkle_view *>(user);
    were_object_pointer<sparkle_view> view__(view);

    ANativeWindow *window;

    if (surface)
        window = ANativeWindow_fromSurface(env, surface);
    else
        window = nullptr;

    fprintf(stdout, "Got window! %p\n", window);

    were::emit(view__, &sparkle_view::surface_changed, window);
}
