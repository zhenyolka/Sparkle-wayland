#ifndef SPARKLE_PLATFORM_SURFACE_H
#define SPARKLE_PLATFORM_SURFACE_H

#include "were_object.h"

class sparkle_platform;
class sparkle_surface;
class were_surface;
class sparkle_keyboard;
class sparkle_pointer;
class sparkle_touch;


class sparkle_damage // XXX3 Move elsewhere
{
public:
    sparkle_damage() :
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

    void limit(int width, int height)
    {
        if (x1_ < 0)
            x1_ = 0;
        if (y1_ < 0)
            y1_ = 0;
        if (x2_ > width)
            x2_ = width;
        if (y2_ > height)
            y2_ = height;
    }

private:
    int x1_;
    int y1_;
    int x2_;
    int y2_;
    bool damaged_;
};

class sparkle_platform_surface : public were_object
{
public:
    ~sparkle_platform_surface();
    sparkle_platform_surface(were_object_pointer<sparkle_platform> platform, were_object_pointer<sparkle_surface> surface);

    void register_keyboard(were_object_pointer<sparkle_keyboard> keyboard);
    void register_pointer(were_object_pointer<sparkle_pointer> pointer);
    void register_touch(were_object_pointer<sparkle_touch> touch);

private:
    void commit(bool full = false);

private:
    were_object_pointer<sparkle_surface> surface_;
    were_object_pointer<were_surface> ws_;
    struct wl_resource *buffer_; // XXX2 Temporary
    struct wl_resource *callback_; // XXX2 Temporary
    sparkle_damage damage_;
    bool no_damage_;
};

#endif // SPARKLE_PLATFORM_SURFACE_H
