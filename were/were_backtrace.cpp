#include "were_backtrace.h"
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <exception>
#include <unwind.h>
#include <dlfcn.h>
#include <cxxabi.h>


were_backtrace::~were_backtrace()
{
}

were_backtrace::were_backtrace() :
    enabled_(false)
{
}

void were_backtrace::enable()
{
    if (enabled_)
        return;

    // XXX2
    std::set_terminate([]()
    {
        auto e = std::current_exception();

        try
        {
            std::rethrow_exception(e);
        }
        catch (const std::exception &e)
        {
            fprintf(stderr, "%s\n", e.what());
        }
        catch (...)
        {
        }

        raise(SIGABRT);
    });

    signal(SIGABRT, handler);
    signal(SIGSEGV, handler);

    enabled_ = true;
}

void were_backtrace::handler(int n)
{
    const char *name = nullptr;
    switch(n)
    {
        case SIGABRT: name = "SIGABRT";  break;
        case SIGSEGV: name = "SIGSEGV";  break;
    }

    if (name)
        fprintf(stderr, "Caught signal %d (%s)\n", n, name);

    print_backtrace();

    std::_Exit(-1);
}

struct backtrace_state
{
    void **current;
    void **end;
};

_Unwind_Reason_Code unwind_callback(struct _Unwind_Context *context, void *arg)
{
    struct backtrace_state *state = (struct backtrace_state *)arg;

    _Unwind_Ptr pc = _Unwind_GetIP(context);

    if (pc)
    {
        if (state->current == state->end)
            return _URC_END_OF_STACK;
        else
            *state->current++ = (void *)(pc);
    }

    return _URC_NO_REASON;
}

void were1_backtrace_print()
{
    const int max = 100;
    void *buffer[max];

    struct backtrace_state state;
    state.current = buffer;
    state.end = buffer + max;

    _Unwind_Backtrace(unwind_callback, &state);

    int count = (int)(state.current - buffer);

    for (int i = 0; i < count; ++i)
    {
        const void *addr = buffer[i];
        const char *symbol = "";

        Dl_info info;
        if (dladdr(addr, &info) && info.dli_sname)
            symbol = info.dli_sname;

        int status = 0;
        char *demangled = __cxxabiv1::__cxa_demangle(symbol, 0, 0, &status);

        fprintf(stdout, "%03d: 0x%p %s\n", i, addr,
            (demangled != NULL && status == 0) ? demangled : symbol);

        if (demangled != NULL)
            free(demangled);
    }
}

void were_backtrace::print_backtrace()
{
    were1_backtrace_print();
}
