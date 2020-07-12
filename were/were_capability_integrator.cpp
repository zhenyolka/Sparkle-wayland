#include "were_capability_integrator.h"

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
    for (auto it = integrators_.begin(); it != integrators_.end(); ++it)
        (*it)();
    integrators_.clear();
}

void were_capability_integrator::disintegrate()
{
    for (auto it = disintegrators_.rbegin(); it != disintegrators_.rend(); ++it)
        (*it)();
    integrators_.clear();
}
