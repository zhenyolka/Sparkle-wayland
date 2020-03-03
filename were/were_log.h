#ifndef WERE_LOG_H
#define WERE_LOG_H

#include "were.h"


class were_log : public were_object, public were_thread_fd_listener
{
public:
    ~were_log();
    were_log();

    void capture_stdout();
    void enable_stdout();
    void enable_file(const std::string &path);

    void message(const char *format, va_list ap);
    void message(const char *format, ...);

signals:
    were_signal<void (std::vector<char> text)> text;

private:
    void event(uint32_t events);

private:
    int fd_;
    int stdout1_;
    int stderr1_;
};

void log(const char *format, ...);

#endif // WERE_LOG_H
