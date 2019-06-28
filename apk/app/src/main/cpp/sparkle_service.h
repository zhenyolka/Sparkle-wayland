#ifndef SPARKLE_SERVICE_H
#define SPARKLE_SERVICE_H

#include "were_object_2.h"
#include "sparkle_java_object.h"

class sparkle_service_fd_listener
{
public:
    virtual void event() = 0;
};

class sparkle_service : public were_object_2, public sparkle_java_object
{
public:
    ~sparkle_service();
    sparkle_service(JNIEnv *env, jobject instance);

    void add_fd_listener(int fd, sparkle_service_fd_listener *listener);
    void remove_fd_listener(int fd);

    int display_width();
    int display_height();
};

#endif // SPARKLE_SERVICE_H
