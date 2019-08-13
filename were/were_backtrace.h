#ifndef WERE_BACKTRACE_H
#define WERE_BACKTRACE_H


class were_backtrace
{
public:

    static were_backtrace &instance()
    {
        static were_backtrace instance;
        return instance;
    }

private:
    ~were_backtrace();
    were_backtrace();

public:
    were_backtrace(const were_backtrace &other) = delete;
    void operator=(const were_backtrace &other) = delete;

public:
    void enable();
    static void print_backtrace();

private:
    static void handler(int n);

private:
    bool enabled_;
};

#endif // WERE_BACKTRACE_H
