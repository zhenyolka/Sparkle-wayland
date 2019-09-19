#ifndef SPARKLE_VIEW_H
#define SPARKLE_VIEW_H

#include "were_platform_surface.h"
#include "sparkle_java_object.h"
#include "sparkle_service.h"


extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_surface_1changed(JNIEnv *env, jobject instance, jlong user, jobject surface);

class ANativeWindow;

class sparkle_view : public were_platform_surface, public sparkle_java_object
{
    friend
    JNIEXPORT void JNICALL
    Java_com_sion_sparkle_SparkleView_surface_1changed(JNIEnv *env, jobject instance, jlong user, jobject surface);

public:
    ~sparkle_view();
    sparkle_view(JNIEnv *env, were_object_pointer<sparkle_service> service, int width, int height, int format);

    void set_enabled(bool enabled);
    void set_visible(bool visible);

    int width() const { return width_; }
    int height() const { return height_; }
    void set_size(int width, int height);
    bool lock(char **data, int *x1, int *y1, int *x2, int *y2, int *stride);
    bool unlock_and_post();

private:
    void set_window(ANativeWindow *window);

private:
    int width_;
    int height_;
    int format_;
    ANativeWindow *window_;
};

#endif // SPARKLE_VIEW_H
