#ifndef WERE_LOG_H
#define WERE_LOG_H

#include "were_slot.h"
#include <cstdarg>
#include <functional>
#include <vector>
#include <string>

class were_log
{
public:

    ~were_log();
    were_log();

    void add_logger(const std::function<void (std::vector<char> data)> &f) { loggers_.push_back(f); }
    void enable_fd(int fd);
    void enable_file(const std::string &path);

    void message(const std::vector<char> &data);
    void message(const char *format, va_list ap);
    void message(const char *format, ...);

private:
    std::vector<std::function<void (std::vector<char> data)>> loggers_;
};

void log(const std::vector<char> &data);
void log(const char *format, ...);

int stdout_capture();
void stdout_restore();
int original_stdout();

#endif // WERE_LOG_H
