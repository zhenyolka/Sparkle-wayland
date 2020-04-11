#ifndef WERE_SIGNAL_HANDLER_H
#define WERE_SIGNAL_HANDLER_H

#include "were.h"


class were_signal_handler : virtual public were_object, public were_thread_fd_listener
{
public:
    ~were_signal_handler();
    were_signal_handler();

signals:
    were_signal<void (uint32_t number)> signal;

private:
    void event(uint32_t events);

private:
    int fd_;
};

#endif // WERE_SIGNAL_HANDLER_H
