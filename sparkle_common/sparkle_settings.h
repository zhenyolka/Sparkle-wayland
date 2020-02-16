#ifndef SPARKLE_SETTINGS_H
#define SPARKLE_SETTINGS_H

#include "were_object.h"
#include <string>
#include <variant>
#include <map>
#include <regex>


struct sparkle_settings_handler
{
    std::regex re;
    std::function<void (const std::smatch &match)> f;
};

class sparkle_settings : public were_object
{
public:
    ~sparkle_settings();
    sparkle_settings();

    void access() const override {}

    void load(const std::string &path);

    template <typename T>
    T get(const std::string &key, const T &default_value)
    {
        auto it = settings_.find(key);
        if (it == settings_.end())
            return default_value;

        return std::get<T>(it->second);
    }

private:
    void register_handler(const std::string &pattern,
        const std::function<void (const std::smatch &match)> &handler);
    void process_line(const std::string &line);

private:
    std::map<std::string, std::variant<std::string, bool, int, double>> settings_;
    std::vector<sparkle_settings_handler> handlers_;
};

#endif // SPARKLE_SETTINGS_H
