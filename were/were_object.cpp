#include "were_object.h"
#include "were_thread.h"

static const char *state_initial = "INITIAL";
static const char *state_integrated = "INTEGRATED";
static const char *state_disintegrated = "DISINTEGRATED";

std::string were_object::dump() const
{
    const char *state__ = nullptr;

    if (state() == integration_state::initial)
        state__ = state_initial;
    else if (state() == integration_state::integrated)
        state__ = state_integrated;
    else if (state() == integration_state::disintegrated)
        state__ = state_disintegrated;

    char buffer[1024]; // NOLINT
    snprintf(buffer, 1024, "%-20p%-45.44s%-5d%-10s", this, typeid(*this).name(), reference_count(), state__);

    return std::string(buffer);
}
