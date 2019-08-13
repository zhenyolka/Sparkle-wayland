#ifndef SPARKLE_ANDROID_LOGGER_H
#define SPARKLE_ANDROID_LOGGER_H

#include <string>

class sparkle_android_logger
{
public:

    static sparkle_android_logger &instance()
    {
        static sparkle_android_logger instance;
        return instance;
    }

private:
    ~sparkle_android_logger();
    sparkle_android_logger();

public:
    sparkle_android_logger(const sparkle_android_logger &other) = delete;
    void operator=(const sparkle_android_logger &other) = delete;

public:
    void redirect_output(const std::string &file);

private:
    bool redirected_;
};

#endif // SPARKLE_ANDROID_LOGGER_H
