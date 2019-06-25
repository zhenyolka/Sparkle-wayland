#include "were_connect.h"

namespace were
{

uint64_t next_id_ = 0;

uint64_t next_id()
{
    return next_id_++;
}

};
