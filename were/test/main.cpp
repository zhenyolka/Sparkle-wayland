#include <cstdio>
#include <iostream>
#include "were_matrix.h"

int main(int argc, char *argv[])
{
    float angle_of_view = 60;
    float near = 0.1;
    float far = 100;
    float b, t, l, r;
    float aspect_ratio = 1280.0f / 720.0f;

    gluPerspective(angle_of_view, aspect_ratio, near, far, b, t, l, r);
    were_matrix_4x4<float> projection = glFrustum(b, t, l, r, near, far);
    were_matrix_4x4<float> rotation = rotation_matrix(0, 0, 10);
    were_matrix_4x4<float> final = rotation * projection;



    std::cout << projection << std::endl;
    std::cout << rotation << std::endl;
    std::cout << final << std::endl;


    return 0;
}
