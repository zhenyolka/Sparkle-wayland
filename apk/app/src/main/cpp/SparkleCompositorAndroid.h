#ifndef SPARKLE_COMPOSITOR_ANDROID_H
#define SPARKLE_COMPOSITOR_ANDROID_H

#include "Sparkle.h"

class SparkleService;
class SparkleCompositorSurfaceAndroid;

class SparkleCompositorAndroid : public WereLinked
{
public:
    ~SparkleCompositorAndroid();
    SparkleCompositorAndroid(Sparkle *sparkle, SparkleService *service);

signals:
    WereSignal<void (SparkleCompositorSurfaceAndroid *android_surface)> surfaceCreated;

private:
    Sparkle *sparkle_;
    SparkleService *service_;

    SparkleCompositorSurfaceAndroid *pointerFocus_;
    SparkleCompositorSurfaceAndroid *keyboardFocus_;
};

#endif // SPARKLE_COMPOSITOR_ANDROID_H
