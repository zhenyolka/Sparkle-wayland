#ifndef WERE_VECTOR_H
#define WERE_VECTOR_H

#include <array>

template <typename T, std::size_t N>
class were_vector
{
public:
    ~were_vector() {}
    were_vector() : data_({}) {}
    explicit were_vector(const std::array<T, N> &data) : data_(data) {}

    const T &value(std::size_t n) const { return data_.at(n); }
    T &value(std::size_t n) { return data_.at(n); }

    were_vector operator+(const were_vector &other)
    {
        were_vector result;
        for (std::size_t i = 0; i < result.data_.size(); ++i)
        {
            result.data_.at(i) = data_.at(i) + other.data_.at(i);
        }
        return result;
    }

    were_vector operator-(const were_vector &other)
    {
        were_vector result;
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
class were_vector_2 : public were_vector<T, 2>
{
public:
    ~were_vector_2() {}
    were_vector_2() {}
    were_vector_2(T x, T y) : were_vector<T, 2>({x, y}) {}
    explicit were_vector_2(const were_vector<T, 2> &other) : were_vector<T, 2>(other) {}

    T &x() { return this->value(0); }
    T &y() { return this->value(1); }
};

#endif // WERE_VECTOR_H
