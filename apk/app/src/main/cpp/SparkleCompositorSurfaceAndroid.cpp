#include "SparkleCompositorSurfaceAndroid.h"
#include "SparkleService.h"
#include "SparkleView.h"
#include <android/native_window.h>


#define WINDOW_FORMAT 5 // Undocumented


SparkleCompositorSurfaceAndroid::~SparkleCompositorSurfaceAndroid()
{
    sparkle_debug_method(sparkle_debug_life);

    destroyed();

    setVisible(false);

    service_->destroyView(view_);
}

SparkleCompositorSurfaceAndroid::SparkleCompositorSurfaceAndroid(Sparkle *sparkle, Sparkle_wl_surface *surface, SparkleService *service) :
    SparkleCompositorSurface(sparkle, surface), service_(service), window_(nullptr)
{
    sparkle_debug_method(sparkle_debug_life);

    view_ = service_->createView();

    view_->surfaceChanged.connect([this](ANativeWindow *window)
    {
        window_ = window;
        commit();
    })->link(this, true);

    view_->mouseButtonPress.connect(&mouseButtonPress);
    view_->mouseButtonRelease.connect(&mouseButtonRelease);
    view_->mousePointerMotion.connect(&mousePointerMotion);
    //view_->mousePointerEnter.connect(&mousePointerEnter);
    //view_->mousePointerLeave.connect(&mousePointerLeave);
    view_->keyboardKeyPress.connect(&keyboardKeyPress);
    view_->keyboardKeyRelease.connect(&keyboardKeyRelease);
    view_->touchDown.connect(&touchDown);
    view_->touchUp.connect(&touchUp);
    view_->touchMotion.connect(&touchMotion);

    service_->showAll.connect([this](){setVisible(true);})->link(this, true);
    service_->hideAll.connect([this](){setVisible(false);})->link(this, true);

    setVisible(true);
}

void SparkleCompositorSurfaceAndroid::makeVisible(bool visible)
{
    if (visible)
        view_->setVisible(true);
    else
        view_->setVisible(false);
}

void SparkleCompositorSurfaceAndroid::commit_shm(struct wl_shm_buffer *shm_buffer)
{
    if (window_ == nullptr)
        return;

    int width = wl_shm_buffer_get_width(shm_buffer);
    int height = wl_shm_buffer_get_height(shm_buffer);
    int format = wl_shm_buffer_get_format(shm_buffer);
    int stride = wl_shm_buffer_get_stride(shm_buffer);
    void *data = wl_shm_buffer_get_data(shm_buffer);

    if (format == WL_SHM_FORMAT_ARGB8888 || format == WL_SHM_FORMAT_XRGB8888)
    {
        int w_width = ANativeWindow_getWidth(window_);
        int w_height = ANativeWindow_getHeight(window_);
        int w_format = ANativeWindow_getFormat(window_);
        ANativeWindow_setBuffersGeometry(window_, w_width, w_height, WINDOW_FORMAT);



        ANativeWindow_Buffer buffer;

        ARect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = w_width;
        rect.bottom = w_height;


        ANativeWindow_lock(window_, &buffer, &rect);

        int rows = 0;
        if (height < buffer.height)
            rows = height;
        else
            rows = buffer.height;

        int line = 0;
        if (stride < buffer.stride*4)
            line = stride;
        else
            line = buffer.stride*4;

        for (int row = 0; row < rows; ++row)
            std::memcpy((char *)buffer.bits + buffer.stride * 4 * row, (char *)data + stride * row, line);

        ANativeWindow_unlockAndPost(window_);

    }
    else
        sparkle_message("WARNING: unsupported format %d\n", format);
}
