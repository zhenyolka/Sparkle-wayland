#ifndef WERE_CAPABILITY_DEBUG_H
#define WERE_CAPABILITY_DEBUG_H

#include <string>

class were_capability_debug
{
public:
    ~were_capability_debug();
    were_capability_debug();

    virtual std::string dump() const
    {
        return std::string("-");
    }
};

#endif // WERE_CAPABILITY_DEBUG_H
