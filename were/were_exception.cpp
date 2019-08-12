#include "were_exception.h"
#include <cstdarg>


were_exception::~were_exception()
{
    delete what_;
}

were_exception::were_exception()
{
    what_ = new char[1];
    what_[0] = 0x00;
}

were_exception::were_exception(const char *format, ...)
{
    what_ = new char[1024];

    va_list ap;
    va_start(ap, format);
    vsnprintf(what_, 1024, format, ap);
    va_end(ap);
}
