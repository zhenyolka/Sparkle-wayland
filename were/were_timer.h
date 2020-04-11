#ifndef WERE_TIMER_H
#define WERE_TIMER_H

#include "were.h"


class were_timer : virtual public were_object, public were_thread_fd_listener
{
public:
    ~were_timer();
    were_timer(int interval, bool single_shot = false);

    void start();
    void stop();

signals:
    were_signal<void ()> timeout;

private:
    void event(uint32_t events);

private:
    int interval_;
    bool single_shot_;
    int fd_;
};

#endif // WERE_TIMER_H
