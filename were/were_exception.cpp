#include "were_exception.h"
#include <cstdarg>
#include <cstdio>

#ifdef __ANDROID__
#include <unwind.h>
#include <dlfcn.h>
#include <cxxabi.h>
#endif

#ifdef __ANDROID__

struct android_backtrace_state
{
    void **current;
    void **end;
};

_Unwind_Reason_Code android_unwind_callback(struct _Unwind_Context *context, void *arg)
{
    android_backtrace_state *state = (android_backtrace_state *)arg;
    uintptr_t pc = _Unwind_GetIP(context);
    if (pc)
    {
        if (state->current == state->end)
            return _URC_END_OF_STACK;
        else
            *state->current++ = reinterpret_cast<void *>(pc);
    }

    return _URC_NO_REASON;
}

void dump_stack()
{
    fprintf(stdout, "android stack dump\n");

    const int max = 100;
    void *buffer[max];

    android_backtrace_state state;
    state.current = buffer;
    state.end = buffer + max;

    _Unwind_Backtrace(android_unwind_callback, &state);

    int count = (int)(state.current - buffer);

    for (int idx = 0; idx < count; idx++)
    {
        const void *addr = buffer[idx];
        const char *symbol = "";

        Dl_info info;
        if (dladdr(addr, &info) && info.dli_sname)
            symbol = info.dli_sname;

        int status = 0;
        char *demangled = __cxxabiv1::__cxa_demangle(symbol, 0, 0, &status);

        fprintf(stdout, "%03d: 0x%p %s\n",
                idx,
                addr,
                (NULL != demangled && 0 == status) ?
                demangled : symbol);

        if (NULL != demangled)
            free(demangled);
    }

    fprintf(stdout, "android stack dump done\n");
}

#else

void dump_stack()
{
}

#endif


were_exception::~were_exception()
{
    delete what_;
}

were_exception::were_exception()
{
    what_ = new char[1];
    what_[0] = 0x00;

    dump_stack();
}

were_exception::were_exception(const char *format, ...)
{
    what_ = new char[1024];

    va_list ap;
    va_start(ap, format);
    vsnprintf(what_, 1024, format, ap);
    va_end(ap);

    dump_stack();
}
