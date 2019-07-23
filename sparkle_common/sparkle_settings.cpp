#include "sparkle_settings.h"
#include "were_exception.h"

sparkle_settings::~sparkle_settings()
{
}

sparkle_settings::sparkle_settings() :
    depth_(0)
{
    load();
}

void sparkle_settings::start_element(void *userData, const XML_Char *name, const XML_Char **atts)
{
    sparkle_settings *instance = reinterpret_cast<sparkle_settings *>(userData);

    std::string name__(name);

    if (name__ == "sparkle")
    {
        fprintf(stdout, "Settings:\n");
        for (int i = 0; atts[i]; i += 2)
        {
            fprintf(stdout, "  %s = %s\n", atts[i], atts[i + 1]);
            instance->settings_.insert(std::make_pair(std::string(atts[i]), std::string(atts[i + 1])));
        }
    }

    instance->depth_ += 1;
}

void sparkle_settings::end_element(void *userData, const XML_Char *name)
{
    sparkle_settings *instance = reinterpret_cast<sparkle_settings *>(userData);

    instance->depth_ -= 1;
}

void sparkle_settings::load()
{
    char buf[BUFSIZ];

#ifdef __ANDROID__
    FILE *file = fopen("/data/data/com.sion.sparkle/settings.xml", "r"); // XXX2
#else
    FILE *file = fopen("settings.xml", "r");
#endif
    if (file == nullptr)
        return;

    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser, this);
    XML_SetElementHandler(parser, start_element, end_element);

    int done;
    do
    {
        size_t len = fread(buf, 1, sizeof(buf), file);
        done = len < sizeof(buf);

        if (XML_Parse(parser, buf, (int)len, done) == XML_STATUS_ERROR)
        {
            fprintf(stderr, "%s at line %lu\n", XML_ErrorString(XML_GetErrorCode(parser)), XML_GetCurrentLineNumber(parser));
            return;
        }

    } while (!done);

    XML_ParserFree(parser);

    fclose(file);
}

std::string sparkle_settings::get_string(const std::string &key, const std::string &default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return it->second;
}

bool sparkle_settings::get_bool(const std::string &key, bool default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    if (it->second == "true")
        return true;
    else if (it->second == "false")
        return false;
    else
        throw were_exception(WE_SIMPLE);
}

int sparkle_settings::get_int(const std::string &key, int default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return std::stoi(it->second);
}

float sparkle_settings::get_float(const std::string &key, float default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return std::stof(it->second);
}
