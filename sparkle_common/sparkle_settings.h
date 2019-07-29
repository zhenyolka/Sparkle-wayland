#ifndef SPARKLE_SETTINGS_H
#define SPARKLE_SETTINGS_H

#include "were_object_2.h"
#include <string>
#include <map>


class sparkle_settings : public were_object_2
{
public:
    ~sparkle_settings();
    sparkle_settings();

    std::string get_string(const std::string &key, const std::string &default_value);
    bool get_bool(const std::string &key, bool default_value);
    int get_int(const std::string &key, int default_value);
    float get_float(const std::string &key, float default_value);

private:
    void load();

private:
    std::map<std::string, std::string> settings_;
};

#endif // SPARKLE_SETTINGS_H
