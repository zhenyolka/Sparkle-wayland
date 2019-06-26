#ifndef SPARKLE_COMPOSITOR_SURFACE_ANDROID_H
#define SPARKLE_COMPOSITOR_SURFACE_ANDROID_H

#include "Sparkle.h"
#include "SparkleCompositorSurface.h"

class SparkleService;
class SparkleView;
class ANativeWindow;

class SparkleCompositorSurfaceAndroid : public SparkleCompositorSurface
{
public:
    ~SparkleCompositorSurfaceAndroid();
    SparkleCompositorSurfaceAndroid(Sparkle *sparkle, Sparkle_wl_surface *surface, SparkleService *service);

signals:
    WereSignal<void ()> destroyed;

private:
    void makeVisible(bool visible);
    void commit_shm(struct wl_shm_buffer *shm_buffer);

private:
    SparkleService *service_;
    SparkleView *view_;
    ANativeWindow *window_;
};

#endif // SPARKLE_COMPOSITOR_SURFACE_ANDROID_H
