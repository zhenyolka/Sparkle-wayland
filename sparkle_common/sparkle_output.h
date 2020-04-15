#ifndef SPARKLE_OUTPUT_H
#define SPARKLE_OUTPUT_H

#include "sparkle.h"
#include "generated/sparkle_wl_output.h"

class sparkle_output : public sparkle_wl_output
{
public:
    sparkle_output(struct wl_client *client, int version, uint32_t id, were_pointer<sparkle_display> display) :
        sparkle_wl_output(client, version, id)
    {
    }
};

#endif // SPARKLE_OUTPUT_H
