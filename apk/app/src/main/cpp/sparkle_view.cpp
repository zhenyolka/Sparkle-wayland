#include "sparkle_view.h"
#include <android/native_window_jni.h>

sparkle_view::~sparkle_view()
{
}

sparkle_view::sparkle_view(JNIEnv *env, were_object_pointer<sparkle_service> service) :
    sparkle_java_object(env, "com/sion/sparkle/SparkleView", "(Lcom/sion/sparkle/SparkleService;J)V", service->object1(), jlong(this))
{
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_surface_1changed(JNIEnv *env, jobject instance, jlong user, jobject surface)
{
    sparkle_view *view = reinterpret_cast<sparkle_view *>(user);
    were_object_pointer<sparkle_view> view__(view);

    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);

    fprintf(stdout, "Got window! %p\n", window);

    were::emit(view__, &sparkle_view::surface_changed, window);
}
