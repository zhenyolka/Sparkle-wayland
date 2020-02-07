#ifndef SPARKLE_SETTINGS_H
#define SPARKLE_SETTINGS_H

#include "were_object.h"
#include <string>
#include <variant>
#include <map>


class sparkle_settings : public were_object
{
public:
    ~sparkle_settings();
    sparkle_settings(const std::string &file);

    template <typename T>
    T get(const std::string &key, const T &default_value)
    {
        auto it = settings_.find(key);
        if (it == settings_.end())
            return default_value;

        return std::get<T>(it->second);
    }

private:
    std::map<std::string, std::variant<std::string, bool, int, double>> settings_;
};

#endif // SPARKLE_SETTINGS_H
