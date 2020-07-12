#include "were_capability_collapse.h"

were_capability_collapse::were_capability_collapse()
{
}

void were_capability_collapse::collapse()
{
    destroyed.emit();
}
