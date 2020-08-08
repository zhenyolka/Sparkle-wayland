#include "were_connect.h"
#include <atomic>

static std::atomic<uint64_t> next_id_ = 0;

namespace were
{
    uint64_t next_id()
    {
        return next_id_++;
    }
} // namespace were
