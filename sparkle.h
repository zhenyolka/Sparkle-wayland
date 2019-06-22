#ifndef SPARKLE_H
#define SPARKLE_H

#include "were_object_2.h"
#include "were_thread.h" // XXX

class sparkle : public were_object_2, public were_thread_fd_listener
{
public:
    ~sparkle();
    sparkle();

private:
    void event(uint32_t events);

private:
    struct wl_display *display_;
};

#endif // SPARKLE_H
