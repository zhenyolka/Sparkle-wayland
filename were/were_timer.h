#ifndef WERE_TIMER_H
#define WERE_TIMER_H

#include "were.h"

class were_fd;

class were_timer : virtual public were_object
{
public:
    ~were_timer() override;
    explicit were_timer(int interval, bool single_shot = false);

    void start();
    void stop();

signals:
    were_signal<void ()> timeout;

private:
    int interval_;
    bool single_shot_;
    were_pointer<were_fd> fd_;
};

#endif // WERE_TIMER_H
