#include "were_backtrace.h"
#include "were_log.h"
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
            log("%s\n", e.what());
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
    switch (n)
    {
        case SIGABRT: name = "SIGABRT";  break;
        case SIGSEGV: name = "SIGSEGV";  break;
        default: name = "?"; break;
    }

    if (name)
        log("Caught signal %d (%s)\n", n, name);

    print_backtrace();

    std::_Exit(-1);
}

class backtrace_state
{
public:
    std::vector<_Unwind_Ptr> list_;
};

_Unwind_Reason_Code unwind_callback(struct _Unwind_Context *context, void *arg)
{
    backtrace_state *state = static_cast<backtrace_state *>(arg);

    _Unwind_Ptr pc = _Unwind_GetIP(context);

    if (pc)
    {
        if (state->list_.size() < 100)
        {
            state->list_.push_back(pc);
        }
        else
        {
            return _URC_END_OF_STACK;
        }
    }

    return _URC_NO_REASON;
}

void were_backtrace::print_backtrace()
{
    backtrace_state state;
    _Unwind_Backtrace(unwind_callback, &state);

    for (unsigned int i = 0; i < state.list_.size(); ++i)
    {
        const void *addr = reinterpret_cast<void *>(state.list_[i]);
        const char *symbol = "";

        Dl_info info;
        if (dladdr(addr, &info) && info.dli_sname)
            symbol = info.dli_sname;

        int status = 0;
        char *demangled = __cxxabiv1::__cxa_demangle(symbol, nullptr, nullptr, &status);

        log("%03d: 0x%p %s\n", i, addr, (demangled != nullptr && status == 0) ? demangled : symbol);

        if (demangled != nullptr)
            free(demangled); // NOLINT
    }
}
