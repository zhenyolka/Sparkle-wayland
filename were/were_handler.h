#ifndef WERE_HANDLER_H
#define WERE_HANDLER_H

#include "were_object.h"
#include <queue>
#include <mutex>

class were_fd;

class were_handler : public were_object
{
public:
    ~were_handler();
    were_handler();

    void process_queue();
    void post(const std::function<void ()> &call);

private:
    void event(uint32_t events);

private:
    were_pointer<were_fd> fd_;
    std::queue< std::function<void ()> > call_queue_;
    std::mutex call_queue_mutex_;
};

#endif // WERE_HANDLER_H
