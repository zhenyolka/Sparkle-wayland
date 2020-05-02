#ifndef WERE_LOG_H
#define WERE_LOG_H

#include "were.h"

class were_fd;

class were_log : virtual public were_object
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
    void event(were_pointer<were_fd> fd, uint32_t events);

private:
    int stdout1_;
    int stderr1_;
};

void log(const char *format, ...);

#endif // WERE_LOG_H
