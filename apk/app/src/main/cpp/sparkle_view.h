#ifndef SPARKLE_VIEW_H
#define SPARKLE_VIEW_H

#include "were_object_2.h"
#include "sparkle_java_object.h"
#include "sparkle_service.h"

class ANativeWindow;

class sparkle_view : public were_object_2, public sparkle_java_object
{
public:
    ~sparkle_view();
    sparkle_view(JNIEnv *env, were_object_pointer<sparkle_service> service);

    int width() const {return width_;}
    int height() const {return height_;}

    void set_enabled(bool enabled);
    void set_visible(bool visible);
    void set_position(int x, int y);
    void set_size(int width, int height);

signals:

    were_signal<void (int code)> key_down;
    were_signal<void (int code)> key_up;

    were_signal<void (int id, int x, int y)> touch_down;
    were_signal<void (int id, int x, int y)> touch_up;
    were_signal<void (int id, int x, int y)> touch_motion;

    were_signal<void (int button)> pointer_button_down;
    were_signal<void (int button)> pointer_button_up;
    were_signal<void (int x, int y)> pointer_motion;
    were_signal<void ()> pointer_enter;
    were_signal<void ()> pointer_leave;

signals:
    were_signal<void (ANativeWindow *window)> surface_changed;

private:
    int width_;
    int height_;
};

#endif // SPARKLE_VIEW_H
