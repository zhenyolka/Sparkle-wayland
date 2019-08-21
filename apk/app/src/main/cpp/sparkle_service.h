#ifndef SPARKLE_SERVICE_H
#define SPARKLE_SERVICE_H

#include "sparkle_java_object.h"

//#define SOUND_THREAD

#ifdef SOUND_THREAD
#include <thread>
#endif

class sparkle;



class sparkle_service : public sparkle_java_object
{
public:
    ~sparkle_service();
    sparkle_service(JNIEnv *env, jobject instance);

    std::string files_dir() const {return files_dir_;}

    void enable_native_loop(int fd);
    void disable_native_loop();

    int display_width();
    int display_height();

private:
#ifdef SOUND_THREAD
    void sound();
#endif

private:
    std::string files_dir_;
    were_object_pointer<sparkle> sparkle_;
#ifdef SOUND_THREAD
    std::thread sound_thread_c_;
#endif
};

#endif // SPARKLE_SERVICE_H
