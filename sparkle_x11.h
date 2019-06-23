#ifndef SPARKLE_X11_H
#define SPARKLE_X11_H

#include "were_object_2.h"

class sparkle;

class sparkle_x11 : public were_object_2
{
public:
    ~sparkle_x11();
    sparkle_x11(were_object_pointer<sparkle> sparkle);

private:
};

#endif // SPARKLE_X11_H
