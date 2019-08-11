#ifndef SPARKLE_SERVICE_H
#define SPARKLE_SERVICE_H

#include "sparkle_java_object.h"

#define SOUND_THREAD

#ifdef SOUND_THREAD
#include <thread>
#endif

class sparkle;
class sparkle_android;
class sparkle_audio;
class were_debug;

class sparkle_service_fd_listener
{
public:
    virtual void event() = 0;
};

class sparkle_service_idle_handler
{
public:
    virtual void idle() = 0;
};

class sparkle_service : public sparkle_java_object, public sparkle_service_fd_listener, public sparkle_service_idle_handler
{
public:
    ~sparkle_service();
    sparkle_service(JNIEnv *env, jobject instance);

    std::string files_dir() const {return files_dir_;}

    void add_fd_listener(int fd, were_object_pointer<sparkle_service_fd_listener> listener);
    void remove_fd_listener(int fd, were_object_pointer<sparkle_service_fd_listener> listener);
    void add_idle_handler(were_object_pointer<sparkle_service_idle_handler> handler);
    void remove_idle_handler(were_object_pointer<sparkle_service_idle_handler> handler);

    int display_width();
    int display_height();

private:
    void event();
    void idle();

#ifdef SOUND_THREAD
    void sound();
#endif

private:
    std::string files_dir_;
    were_object_pointer<were_thread> thread_;
    were_object_pointer<sparkle> sparkle_;
    were_object_pointer<sparkle_android> sparkle_android_;
    were_object_pointer<sparkle_audio> audio_;
    were_object_pointer<were_debug> debug_;
#ifdef SOUND_THREAD
    std::thread sound_thread_c_;
    were_object_pointer<were_thread> sound_thread_;
#endif
};

#endif // SPARKLE_SERVICE_H
