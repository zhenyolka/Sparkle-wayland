#ifndef SPARKLE_VIEW_H
#define SPARKLE_VIEW_H

#include "were.h"
#include "sparkle_java_object.h"
#include "sparkle_service.h"
#include "were_rect.h"


extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_surface_1changed(JNIEnv *env, jobject instance, jlong user, jobject surface);

class ANativeWindow;
class were_surface;

class sparkle_view : virtual public were_object, public sparkle_java_object
{
    friend
    JNIEXPORT void JNICALL
    Java_com_sion_sparkle_SparkleView_surface_1changed(JNIEnv *env, jobject instance, jlong user, jobject surface);

public:
    ~sparkle_view();
    sparkle_view(JNIEnv *env, were_pointer<sparkle_service> service, were_pointer<were_surface> surface);

    were_pointer<were_surface> surface() const { return surface_; }

    int width() const { return width_; }
    int height() const { return height_; }
    void set_visible(bool visible);
    void set_position(int x, int y);
    void set_size(int width, int height);

private:
    void set_window(ANativeWindow *window);
    void update(bool full = false);

private:
    were_pointer<were_surface> surface_;
    int width_;
    int height_;
    int format_;
    ANativeWindow *window_;
    were_rect<int> damage_;
    bool no_damage_;
};

#endif // SPARKLE_VIEW_H
