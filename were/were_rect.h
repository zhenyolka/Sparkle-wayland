#ifndef WERE_RECT_H
#define WERE_RECT_H

template <typename T>
class were_rect
{
public:
    were_rect() :
        x1_(0), y1_(0), x2_(0), y2_(0)
    {
    }

    T x1() const { return x1_; }
    T y1() const { return y1_; }
    T x2() const { return x2_; }
    T y2() const { return y2_; }
    T width() const { return x2_ - x1_; }
    T height() const { return y2_ - y1_; }

    bool empty() const
    {
        return x1_ == 0 && y1_ == 0 && x2_ == 0 && y2_ == 0;
    }

    void expand(T x1, T y1, T x2, T y2)
    {
        if (empty())
        {
            x1_ = x1;
            y1_ = y1;
            x2_ = x2;
            y2_ = y2;
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

    void reset()
    {
        x1_ = 0;
        y1_ = 0;
        x2_ = 0;
        y2_ = 0;
    }

    void limit(T width, T height)
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
    T x1_;
    T y1_;
    T x2_;
    T y2_;
};

#endif // WERE_RECT_H
