#ifndef WERE_CAPABILITY_INTEGRATOR_H
#define WERE_CAPABILITY_INTEGRATOR_H

#include <functional>
#include <vector>

class were_capability_integrator
{
public:

    enum class integration_state
    {
        initial, integrated, disintegrated,
    };

    were_capability_integrator();

    void add_integrator(const std::function<void ()> &f);
    void add_disintegrator(const std::function<void ()> &f);
    void integrate();
    void disintegrate();
    integration_state state() const { return state_; }

private:
    std::vector<std::function<void ()>> integrators_;
    std::vector<std::function<void ()>> disintegrators_;
    integration_state state_;
};

#endif // WERE_CAPABILITY_INTEGRATOR_H
