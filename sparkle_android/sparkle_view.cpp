#include "sparkle_view.h"
#include "sparkle_keymap.h"
#include "were_surface.h"
#include "were_upload.h"
#include "sparkle_settings.h"
#include <android/native_window_jni.h>
#include <linux/input-event-codes.h>


static const int button_map[7] = {0, BTN_LEFT, BTN_RIGHT, 0, BTN_MIDDLE, BTN_GEAR_UP, BTN_GEAR_DOWN};

sparkle_view::~sparkle_view()
{
    if (window_ != nullptr)
        ANativeWindow_release(window_);
}

sparkle_view::sparkle_view(JNIEnv *env, were_object_pointer<sparkle_service> service, were_object_pointer<were_surface> surface) :
    sparkle_java_object(env, "com/sion/sparkle/SparkleView", "(Lcom/sion/sparkle/SparkleService;J)V", service->object1(), jlong(this)),
    surface_(surface),
    window_(nullptr)
{
    auto this_wop = make_wop(this);

    reference();

    width_ = 100;
    height_ = 100;

    bool fast = global<sparkle_settings>()->get<bool>("fast", false);
    if (fast)
        format_ = 5;
    else
        format_ = WINDOW_FORMAT_RGBX_8888;

    no_damage_ = global<sparkle_settings>()->get<bool>("no_damage", false);

    were::connect(this_wop, &were_object::destroyed, this_wop, [this_wop]()
    {
        this_wop->call_void_method("collapse", "()V");
        this_wop->unreference();
    });

    were::connect(surface, &were_surface::damage, this_wop, [this_wop](int x, int y, int width, int height)
    {
        this_wop->damage_.expand(x, y, x + width, y + height);
    });

    were::connect(surface, &were_surface::commit, this_wop, [this_wop]()
    {
        this_wop->update(false);
    });
}

void sparkle_view::set_visible(bool visible)
{
    call_void_method("set_visible", "(Z)V", jboolean(visible));
}

void sparkle_view::set_position(int x, int y)
{
    call_void_method("set_position", "(II)V", jint(x), jint(y));
}

void sparkle_view::set_size(int width, int height)
{
    if (width != width_ || height != height_)
    {
        call_void_method("set_size", "(II)V", jint(width), jint(height));
        width_ = width;
        height_ = height;
    }
}

void sparkle_view::set_window(ANativeWindow *window)
{
    auto this_wop = make_wop(this);

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
        {
            fprintf(stdout, "WARNING! Wrong window size (%dx%d, expected %dx%d)\n",
                    w_width, w_height, width_, height_);
            //throw were_exception(WE_SIMPLE);
        }

        if (ANativeWindow_setBuffersGeometry(window_, width_, height_, format_) != 0)
            throw were_exception(WE_SIMPLE);

        update(true);
    }
}

void sparkle_view::update(bool full)
{
    void *data = surface_->data();

    if (data == nullptr)
        return;

    if (window_ == nullptr)
        return;

    if (width_ != surface_->width() || height_ != surface_->height())
    {
        set_size(surface_->width(), surface_->height());
        return;
    }

    if (full || no_damage_)
        damage_.expand(0, 0, width_, height_);

    damage_.limit(width_, height_);

    ARect rect;
    rect.left = damage_.x1();
    rect.top = damage_.y1();
    rect.right = damage_.x2();
    rect.bottom = damage_.y2();

    ANativeWindow_Buffer buffer;

    if (ANativeWindow_lock(window_, &buffer, &rect) != 0)
        throw were_exception(WE_SIMPLE);

    if (buffer.width != width_ || buffer.height != height_)
    {
        fprintf(stdout, "WARNING! Wrong buffer size (%dx%d, expected %dx%d)\n",
                    buffer.width, buffer.height, width_, height_);
        //throw were_exception(WE_SIMPLE);
        return;
    }


    if (surface_->format() == were_surface::buffer_format::ARGB8888 && format_ == 5)
    {
        were_upload::uploader[0](buffer.bits, data, surface_->stride(), buffer.stride * 4,
                             rect.left, rect.top, rect.right, rect.bottom);
    }
    else if (surface_->format() == were_surface::buffer_format::ARGB8888 && format_ == WINDOW_FORMAT_RGBX_8888)
    {
        were_upload::uploader[2](buffer.bits, data, surface_->stride(), buffer.stride * 4,
                             rect.left, rect.top, rect.right, rect.bottom);
    }

    ANativeWindow_unlockAndPost(window_);

    damage_.reset();
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
        were::emit(view->surface(), &were_surface::key_down, x);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_key_1up(JNIEnv *env, jobject instance, jlong user, jint code)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    int x = sparkle_keymap[code];
    if (x != 0)
        were::emit(view->surface(), &were_surface::key_up, x);
}

/* Pointer */

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1button_1down(JNIEnv *env, jobject instance, jlong user, jint button)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view->surface(), &were_surface::pointer_button_down, button_map[button]);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1button_1up(JNIEnv *env, jobject instance, jlong user, jint button)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view->surface(), &were_surface::pointer_button_up, button_map[button]);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1motion(JNIEnv *env, jobject instance, jlong user, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view->surface(), &were_surface::pointer_motion, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1enter(JNIEnv *env, jobject instance, jlong user)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view->surface(), &were_surface::pointer_enter);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_pointer_1leave(JNIEnv *env, jobject instance, jlong user)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view->surface(), &were_surface::pointer_leave);
}

/* Touch */

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1down(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view->surface(), &were_surface::touch_down, id, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1up(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view->surface(), &were_surface::touch_up, id, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleView_touch_1motion(JNIEnv *env, jobject instance, jlong user, jint id, jfloat x, jfloat y)
{
    were_object_pointer<sparkle_view> view(reinterpret_cast<sparkle_view *>(user));
    were::emit(view->surface(), &were_surface::touch_motion, id, x, y);
}
