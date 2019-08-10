#ifndef SPARKLE_SETTINGS_H
#define SPARKLE_SETTINGS_H

#include "were_object.h"
#include "were_variant.h"
#include <string>
#include <map>


class sparkle_settings : public were_object
{
public:
    ~sparkle_settings();
    sparkle_settings(const std::string &file);

    std::string get_string(const std::string &key, const std::string &default_value);
    bool get_bool(const std::string &key, bool default_value);
    int get_int(const std::string &key, int default_value);
    double get_float(const std::string &key, double default_value);

private:
    void load();

private:
    std::string file_;
    std::map<std::string, were_variant> settings_;
};

#endif // SPARKLE_SETTINGS_H
