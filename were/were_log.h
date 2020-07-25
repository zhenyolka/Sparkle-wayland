#ifndef WERE_LOG_H
#define WERE_LOG_H

#include <cstdarg>
#include <functional>
#include <vector>
#include <string>

class were_log
{
public:

    static void add_logger(const std::function<void (std::vector<char> data)> &f) { loggers_.push_back(f); }
    static void enable_fd(int fd);
    static void enable_file(const std::string &path);

    static void message(const std::vector<char> &data);
    static void message(const char *format, va_list ap);
    static void message(const char *format, ...);

private:
    static std::vector<std::function<void (std::vector<char> data)>> loggers_;
};


int stdout_capture();
void stdout_restore();
int original_stdout();

#endif // WERE_LOG_H
