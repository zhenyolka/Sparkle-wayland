#include "sparkle_settings.h"
#include "were_exception.h"


sparkle_settings::~sparkle_settings()
{
}

sparkle_settings::sparkle_settings(const std::string &file) :
    file_(file)
{
    load();
}

void sparkle_settings::load()
{
}

std::string sparkle_settings::get_string(const std::string &key, const std::string &default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return it->second.get<std::string>();
}

bool sparkle_settings::get_bool(const std::string &key, bool default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return it->second.get<bool>();
}

int sparkle_settings::get_int(const std::string &key, int default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return it->second.get<double>();
}

double sparkle_settings::get_float(const std::string &key, double default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return it->second.get<double>();
}
