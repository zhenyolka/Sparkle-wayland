#include "sparkle_view.h"
#include "sparkle_keymap.h"
#include "were_surface.h"
#include <android/native_window_jni.h>
#include <linux/input-event-codes.h>


static const int button_map[7] = {0, BTN_LEFT, BTN_RIGHT, 0, BTN_MIDDLE, BTN_GEAR_UP, BTN_GEAR_DOWN};

sparkle_view::~sparkle_view()
{
    if (window_ != nullptr)
        ANativeWindow_release(window_);
}

sparkle_view::sparkle_view(JNIEnv *env, were_object_pointer<sparkle_service> service, int width, int height, int format) :
    sparkle_java_object(env, "com/sion/sparkle/SparkleView", "(Lcom/sion/sparkle/SparkleService;JJJ)V", service->object1(), jlong(width), jlong(height), jlong(this)), window_(nullptr)
{
    MAKE_THIS_WOP

    increment_reference_count();

    width_ = width;
    height_ = height;

    if (format == WERE_SURFACE_FORMAT_ARGB8888)
        format_ = 5;
    else if (format == WERE_SURFACE_FORMAT_ABGR8888)
        format_ = WINDOW_FORMAT_RGBX_8888;
    else
        throw were_exception(WE_SIMPLE);

    were_object::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
    {
        this_wop->collapse1();
    });
}

void sparkle_view::collapse1()
{
    call_void_method("collapse", "()V");
    decrement_reference_count();
}

void sparkle_view::set_visible(bool visible)
{
    call_void_method("set_visible", "(Z)V", jboolean(visible));
}

#if 0
void sparkle_view::set_position(int x, int y)
{
    call_void_method("set_position", "(II)V", jint(x), jint(y));
}
#endif

void sparkle_view::set_size(int width, int height)
{
    if (width != width_ || height != height_)
    {
        call_void_method("set_size", "(II)V", jint(width), jint(height));
        width_ = width;
        height_ = height;
    }
}

bool sparkle_view::lock(char **data, int *x1, int *y1, int *x2, int *y2, int *stride)
{
    if (window_ == nullptr)
        return false;

    ARect rect;
    rect.left = *x1;
    rect.top = *y1;
    rect.right = *x2;
    rect.bottom = *y2;

    ANativeWindow_Buffer buffer;

    if (ANativeWindow_lock(window_, &buffer, &rect) != 0)
        throw were_exception(WE_SIMPLE);

    if (buffer.width != width_ || buffer.height != height_)
        throw were_exception(WE_SIMPLE);

    *x1 = rect.left;
    *y1 = rect.top;
    *x2 = rect.right;
    *y2 = rect.bottom;
    *stride = buffer.stride * 4;
    *data = (char *)buffer.bits;

    return true;
}

bool sparkle_view::unlock_and_post()
{
    ANativeWindow_unlockAndPost(window_);

    return true;
}

void sparkle_view::set_window(ANativeWindow *window)
{
    MAKE_THIS_WOP

    if (window_ != nullptr)
        ANativeWindow_release(window_);

    window_ = window;

    if (window_ != nullptr)
    {
        ANativeWindow_acquire(window_);

        int w_width = ANativeWindow_getWidth(window_);
        int w_height = ANativeWindow_getHeight(window_);
        //int w_format = ANativeWindow_getFormat(window_);

        if (w_width != width_ || w_height != height_)
            throw were_exception(WE_SIMPLE);

        if (ANativeWindow_setBuffersGeometry(window_, w_width, w_height, format_) != 0)
            throw were_exception(WE_SIMPLE);

        were_object::emit(this_wop->callbacks(), &were_surface::expose);
    }
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

    view->set_window(window);

    if (window != nullptr)
        ANativeWindow_release(window);
}

/* Keyboard */

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_key_1down(JNIEnv *env, jobject instance, jlong user, jint code)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    int x = sparkle_keymap[code];
    if (x != 0)
        were_object::emit(view->callbacks(), &were_surface::key_down, x);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_key_1up(JNIEnv *env, jobject instance, jlong user, jint code)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    int x = sparkle_keymap[code];
    if (x != 0)
        were_object::emit(view->callbacks(), &were_surface::key_up, x);
}

/* Pointer */

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1button_1down(JNIEnv *env, jobject instance, jlong user, jint button)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were_object::emit(view->callbacks(), &were_surface::pointer_button_down, button_map[button]);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1button_1up(JNIEnv *env, jobject instance, jlong user, jint button)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were_object::emit(view->callbacks(), &were_surface::pointer_button_up, button_map[button]);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1motion(JNIEnv *env, jobject instance, jlong user, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were_object::emit(view->callbacks(), &were_surface::pointer_motion, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1enter(JNIEnv *env, jobject instance, jlong user)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were_object::emit(view->callbacks(), &were_surface::pointer_enter);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1leave(JNIEnv *env, jobject instance, jlong user)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were_object::emit(view->callbacks(), &were_surface::pointer_leave);
}

/* Touch */

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1down(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were_object::emit(view->callbacks(), &were_surface::touch_down, id, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1up(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were_object::emit(view->callbacks(), &were_surface::touch_up, id, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1motion(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were_object::emit(view->callbacks(), &were_surface::touch_motion, id, x, y);
}
