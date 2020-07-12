#include "were_capability_integrator.h"
#include "were_exception.h"
#include <cstdio>

were_capability_integrator::were_capability_integrator() :
    state_(integration_state::initial)
{
}

void were_capability_integrator::add_integrator(const std::function<void ()> &f)
{
    integrators_.push_back(f);
}

void were_capability_integrator::add_disintegrator(const std::function<void ()> &f)
{
    disintegrators_.push_back(f);
}

void were_capability_integrator::integrate()
{
    if (state_ != integration_state::initial)
        throw were_exception(WE_SIMPLE);

    for (auto it = integrators_.begin(); it != integrators_.end(); ++it)
        (*it)();
    integrators_.clear();

    state_ = integration_state::integrated;
}

void were_capability_integrator::disintegrate()
{
    if (state_ != integration_state::integrated)
    {
        fprintf(stdout, "%s already disintegrated\n", typeid(*this).name());
        return; // XXX1
        //throw were_exception(WE_SIMPLE);
    }

    for (auto it = disintegrators_.rbegin(); it != disintegrators_.rend(); ++it)
        (*it)();
    integrators_.clear();

    state_ = integration_state::disintegrated;
}
