#ifndef SPARKLE_SERVICE_H
#define SPARKLE_SERVICE_H

#include "SparkleJavaObject.h"
#include "WereSignal.h"
#include <functional>

class Sparkle;
class SparkleCompositorAndroid;
class SparkleView;

class SparkleService : public SparkleJavaObject
{
public:
    ~SparkleService();
    SparkleService(JNIEnv *env, jobject instance);

    void setNativeFd(int fd);
    void unsetNativeFd();

    SparkleView *createView();
    void destroyView(SparkleView *view);

signals:
    WereSignal<void ()> nativeEvent;
    WereSignal<void (int width, int height)> displaySize;
    WereSignal<void ()> showAll;
    WereSignal<void ()> hideAll;
    WereSignal<void ()> stopServer;

private:
    Sparkle *sparkle_;
    SparkleCompositorAndroid *compositor_;
};

#endif // SPARKLE_SERVICE_H
