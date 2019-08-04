#ifndef SPARKLE_ANDROID_LOGGER_H
#define SPARKLE_ANDROID_LOGGER_H

class sparkle_android_logger
{
public:
    ~sparkle_android_logger();
    sparkle_android_logger();

    static void redirect();

private:
    static bool redirected_;
};

#endif // SPARKLE_ANDROID_LOGGER_H
