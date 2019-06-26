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
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);

    fprintf(stdout, "Got window! %p\n", window);

    if (window == nullptr)
        return;

        int w_width = ANativeWindow_getWidth(window);
        int w_height = ANativeWindow_getHeight(window);
        int w_format = ANativeWindow_getFormat(window);
        ANativeWindow_setBuffersGeometry(window, w_width, w_height, 5);



        ANativeWindow_Buffer buffer;

        ARect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = w_width;
        rect.bottom = w_height;


        ANativeWindow_lock(window, &buffer, &rect);


        ANativeWindow_unlockAndPost(window);


}
