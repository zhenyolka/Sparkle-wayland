#ifndef WERE_MATRIX_H
#define WERE_MATRIX_H

#include <array>
#include <ostream>
#include <iomanip>
#include <cmath>

template <typename T, std::size_t ROWS, std::size_t COLUMNS>
class were_matrix
{
public:
    ~were_matrix() {}
    were_matrix() : data_({}) {}
    were_matrix(const std::array<std::array<T, COLUMNS>, ROWS> &data) : data_(data) {}

    const T &value(std::size_t column, std::size_t row) const { return data_.at(row).at(column); }
    T &value(std::size_t column, std::size_t row) { return data_.at(row).at(column); }

    were_matrix operator*(const were_matrix &other)
    {
        were_matrix result;
        for (std::size_t row = 0; row < ROWS; ++row)
        {
            for (std::size_t column = 0; column < COLUMNS; ++column)
            {
                result.value(column, row) = 0;
                for (std::size_t k = 0; k < ROWS; ++k)
                {
                    result.value(column, row) += value(k, row) * other.value(column, k);
                }
            }
        }
        return result;
    }

    friend std::ostream &operator<<(std::ostream &stream, const were_matrix &matrix)
    {
        stream << std::setprecision(5);
        stream << std::fixed;

        for (std::size_t row = 0; row < ROWS; ++row)
        {
            for (std::size_t column = 0; column < COLUMNS; ++column)
            {
                stream << std::setw(10) << matrix.value(column, row) << " ";
            }
            stream << std::endl;
        }
        return stream;
    }

private:
    std::array<std::array<T, COLUMNS>, ROWS> data_;
};

template <typename T>
class were_matrix_4x4 : public were_matrix<T, 4, 4>
{
public:
    ~were_matrix_4x4() {}
    were_matrix_4x4() : were_matrix<T, 4, 4>({{{1, 0, 0, 0},
                                               {0, 1, 0, 0},
                                               {0, 0, 1, 0},
                                               {0, 0, 0, 1}}}) {}
    were_matrix_4x4(const std::array<std::array<T, 4>, 4> &data) : were_matrix<T, 4, 4>(data) {}
    were_matrix_4x4(const were_matrix<T, 4, 4> &other) : were_matrix<T, 4, 4>(other) {}
};

inline float to_radian(float x)
{
    return x * M_PI / 180.0f;
}

inline float to_degree(float x)
{
    return x * 180.0f / M_PI;
}

inline void gluPerspective(float angle_of_view, float image_aspect_ratio, float n, float f, float &b, float &t, float &l, float &r)
{
    float scale = tan(angle_of_view * 0.5 * M_PI / 180) * n;
    r = image_aspect_ratio * scale;
    l = -r;
    t = scale;
    b = -t;
}

inline were_matrix_4x4<float>  glFrustum(float b, float t, float l, float r, float n, float f)
{
    were_matrix_4x4<float> result;

    result.value(0, 0) = 2 * n / (r - l);
    result.value(1, 0) = 0;
    result.value(2, 0) = 0;
    result.value(3, 0) = 0;

    result.value(0, 1) = 0;
    result.value(1, 1) = 2 * n / (t - b);
    result.value(2, 1) = 0;
    result.value(3, 1) = 0;

    result.value(0, 2) = (r + l) / (r - l);
    result.value(1, 2) = (t + b) / (t - b);
    result.value(2, 2) = -(f + n) / (f - n);
    result.value(3, 2) = -1;

    result.value(0, 3) = 0;
    result.value(1, 3) = 0;
    result.value(2, 3) = -2 * f * n / (f - n);
    result.value(3, 3) = 0;

    return result;
}

inline were_matrix_4x4<float>  glOrtho(float b, float t, float l, float r, float n, float f)
{
    were_matrix_4x4<float> result;
    result.value(0, 0) = 2 / (r - l);
    result.value(1, 0) = 0;
    result.value(2, 0) = 0;
    result.value(3, 0) = 0;

    result.value(0, 1) = 0;
    result.value(1, 1) = 2 / (t - b);
    result.value(2, 1) = 0;
    result.value(3, 1) = 0;

    result.value(0, 2) = 0;
    result.value(1, 2) = 0;
    result.value(2, 2) = -2 / (f - n);
    result.value(3, 2) = 0;

    result.value(0, 3) = -(r + l) / (r - l);
    result.value(1, 3) = -(t + b) / (t - b);
    result.value(2, 3) = -(f + n) / (f - n);
    result.value(3, 3) = 1;

    return result;
}

inline were_matrix_4x4<float> scale_matrix(float x, float y, float z)
{
    were_matrix_4x4<float> r({{
        {x,          0.0f,       0.0f,       0.0f},
        {0.0f,       y,          0.0f,       0.0f},
        {0.0f,       0.0f,       z,          0.0f},
        {0.0f,       0.0f,       0.0f,       1.0f},
    }});
    return r;
}

inline were_matrix_4x4<float> translation_matrix(float x, float y, float z)
{
    were_matrix_4x4<float> r({{
        {1.0f,       0.0f,       0.0f,       0.0f},
        {0.0f,       1.0f,       0.0f,       0.0f},
        {0.0f,       0.0f,       1.0f,       0.0f},
        {x,          y,          z,          1.0f},
    }});
    return r;
}

inline were_matrix_4x4<float> rotation_matrix(float angle_x, float angle_y, float angle_z)
{
    const float x = to_radian(angle_x);
    const float y = to_radian(angle_y);
    const float z = to_radian(angle_z);

    were_matrix_4x4<float> rx({{
        {1.0f,       0.0f,       0.0f,       0.0f},
        {0.0f,       cosf(x),    sinf(x),    0.0f},
        {0.0f,       -sinf(x),   cosf(x),    0.0f},
        {0.0f,       0.0f,       0.0f,       1.0f},
    }});

    were_matrix_4x4<float> ry({{
        {cosf(y),    0.0f,       sinf(y),    0.0f},
        {0.0f,       1.0f,       0.0f,       0.0f},
        {-sinf(y),   0.0f,       cosf(y),    0.0f},
        {0.0f,       0.0f,       0.0f,       1.0f},
    }});

    were_matrix_4x4<float> rz({{
        {cosf(z),    sinf(z),    0.0f,       0.0f},
        {-sinf(z),   cosf(z),    0.0f,       0.0f},
        {0.0f,       0.0f,       1.0f,       0.0f},
        {0.0f,       0.0f,       0.0f,       1.0f},
    }});

    were_matrix_4x4<float> r = rx * ry * rz;

    return r;
}

#endif // WERE_MATRIX_H
