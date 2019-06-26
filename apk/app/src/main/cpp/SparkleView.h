#ifndef SPARKLE_VIEW_H
#define SPARKLE_VIEW_H

#include "SparkleJavaObject.h"
#include "WereSignal.h"

class ANativeWindow;

class SparkleView : public SparkleJavaObject
{
public:
    ~SparkleView();
    SparkleView(JNIEnv *env, jobject instance);

    void setVisible(bool visible);
    void resize(int width, int height);

signals:
    WereSignal<void (ANativeWindow *window)> surfaceChanged;
    WereSignal<void (int button)> mouseButtonPress;
    WereSignal<void (int button)> mouseButtonRelease;
    WereSignal<void (int x, int y)> mousePointerMotion;
    //WereSignal<void ()> mousePointerEnter;
    //WereSignal<void ()> mousePointerLeave;
    WereSignal<void (int code)> keyboardKeyPress;
    WereSignal<void (int code)> keyboardKeyRelease;
    WereSignal<void (int id, int x, int y)> touchDown;
    WereSignal<void (int id, int x, int y)> touchUp;
    WereSignal<void (int id, int x, int y)> touchMotion;
};

#endif // SPARKLE_VIEW_H
