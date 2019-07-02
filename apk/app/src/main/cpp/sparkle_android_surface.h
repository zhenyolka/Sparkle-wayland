#ifndef SPARKLE_ANDROID_SURFACE_H
#define SPARKLE_ANDROID_SURFACE_H

#include "sparkle.h"

class sparkle_android;
class sparkle_surface;
class sparkle_view;
class sparkle_keyboard;
class sparkle_pointer;
class sparkle_touch;

class ANativeWindow;

class sparkle_surface_damage // XXX Move elsewhere
{
public:
    sparkle_surface_damage() :
        x1_(0), y1_(0), x2_(0), y2_(0), damaged_(false)
    {
    }

    int x1() const {return x1_;}
    int y1() const {return y1_;}
    int x2() const {return x2_;}
    int y2() const {return y2_;}
    int width() const {return x2_ - x1_;}
    int height() const {return y2_ - y1_;}
    bool damaged() const {return damaged_;}

    void add(int x1, int y1, int x2, int y2)
    {
        if (!damaged_)
        {
            x1_ = x1;
            y1_ = y1;
            x2_ = x2;
            y2_ = y2;
            damaged_ = true;
        }
        else
        {
            if (x1 < x1_)
                x1_ = x1;
            if (y1 < y1_)
                y1_ = y1;
            if (x2 > x2_)
                x2_ = x2;
            if (y2 > y2_)
                y2_ = y2;
        }
    }

    void clear()
    {
        x1_ = 0;
        y1_ = 0;
        x2_ = 0;
        y2_ = 0;
        damaged_ = false;
    }

private:
    int x1_;
    int y1_;
    int x2_;
    int y2_;
    bool damaged_;
};

class sparkle_android_surface : public were_object_2
{
public:
    ~sparkle_android_surface();
    sparkle_android_surface(were_object_pointer<sparkle_android> android, were_object_pointer<sparkle_surface> surface);

    were_object_pointer<sparkle_surface> surface() const
    {
        return surface_;
    }

    void register_keyboard(were_object_pointer<sparkle_keyboard> keyboard);
    void register_pointer(were_object_pointer<sparkle_pointer> pointer);
    void register_touch(were_object_pointer<sparkle_touch> touch);

private:
    void commit();

private:
    were_object_pointer<sparkle_surface> surface_;
    were_object_pointer<sparkle_view> view_;
    struct wl_resource *buffer_; // XXX Temporary
    struct wl_resource *callback_; // XX Temporary
    ANativeWindow *window_;
    sparkle_surface_damage damage_;
};

#endif // SPARKLE_ANDROID_SURFACE_H
