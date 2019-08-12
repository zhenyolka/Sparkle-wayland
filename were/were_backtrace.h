#ifndef WERE_BACKTRACE_H
#define WERE_BACKTRACE_H


class were_backtrace
{
public:
    ~were_backtrace();
    were_backtrace();

    static void enable();
    static void print_backtrace();

private:
    static void handler(int n);

private:
    static bool enabled_;
};

#endif // WERE_BACKTRACE_H
