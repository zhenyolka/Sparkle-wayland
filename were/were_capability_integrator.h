#ifndef WERE_CAPABILITY_INTEGRATOR_H
#define WERE_CAPABILITY_INTEGRATOR_H

#include <functional>
#include <vector>

class were_capability_integrator
{
public:
    void add_integrator(const std::function<void ()> &f);
    void add_disintegrator(const std::function<void ()> &f);
    void integrate();
    void disintegrate();

private:
    std::vector<std::function<void ()>> integrators_;
    std::vector<std::function<void ()>> disintegrators_;
};

#endif // WERE_CAPABILITY_INTEGRATOR_H
