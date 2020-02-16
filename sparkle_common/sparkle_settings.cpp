#include "sparkle_settings.h"
#include "were_exception.h"
#include <fstream>
#include <regex>
#include <cstdio>


sparkle_settings::~sparkle_settings()
{
}

sparkle_settings::sparkle_settings(const std::string &path) :
    path_(path)
{
    register_handler("#.*", [this](const std::smatch &match)
    {
    });

    register_handler("(\\w+) = (\\d+);", [this](const std::smatch &match)
    {
        settings_[match.str(1)] = std::stoi(match.str(2));
    });

    register_handler("(\\w+) = (true|false);", [this](const std::smatch &match)
    {
        if (match.str(2) == "true")
            settings_[match.str(1)] = true;
        else if (match.str(2) == "false")
            settings_[match.str(1)] = false;
        else
            throw were_exception(WE_SIMPLE);
    });

    register_handler("(\\w+) = \"(.+)\";", [this](const std::smatch &match)
    {
        settings_[match.str(1)] = match.str(2);
    });
}

void sparkle_settings::register_handler(const std::string &pattern,
    const std::function<void (const std::smatch &match)> &handler)
{
    handlers_.push_back({std::regex(pattern), handler});
}

void sparkle_settings::process_line(const std::string &line)
{
    bool ok = false;

    std::smatch match;

    for (auto &handler : handlers_)
    {
        if (std::regex_match(line, match, handler.re))
        {
            handler.f(match);
            ok = true;
        }
    }

    if (!ok)
        throw were_exception(WE_SIMPLE);
}

void sparkle_settings::load()
{
    mutex_.lock();

    settings_.clear();

    std::ifstream file(path_);

    if (file.is_open())
    {
        std::string line;

        while (std::getline(file, line))
            process_line(line);

        file.close();
    }
    else
        throw were_exception(WE_SIMPLE);

    mutex_.unlock();
}
