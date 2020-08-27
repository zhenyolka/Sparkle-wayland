#ifndef WERE_POINT_H
#define WERE_POINT_H

#include <array>

template <typename T, std::size_t N>
class were_point
{
public:
    ~were_point() {}
    were_point() : data_({}) {}
    were_point(const std::array<T, N> &data) : data_(data) {}

    const T &value(std::size_t n) const { return data_.at(n); }
    T &value(std::size_t n) { return data_.at(n); }

    were_point operator+(const were_point &other)
    {
        were_point result;
        for (std::size_t i = 0; i < result.data_.size(); ++i)
        {
            result.data_.at(i) = data_.at(i) + other.data_.at(i);
        }
        return result;
    }

    were_point operator-(const were_point &other)
    {
        were_point result;
        for (std::size_t i = 0; i < result.data_.size(); ++i)
        {
            result.data_.at(i) = data_.at(i) - other.data_.at(i);
        }
        return result;
    }

private:
    std::array<T, N> data_;
};

template <typename T>
class were_point_2 : public were_point<T, 2>
{
public:
    ~were_point_2() {}
    were_point_2() {}
    were_point_2(T x, T y) : were_point<T, 2>({x, y}) {}
    were_point_2(const were_point<T, 2> &other) : were_point<T, 2>(other) {}

    T &x() { return this->value(0); }
    T &y() { return this->value(1); }
};

#endif // WERE_POINT_H
