#include "sparkle_service.h"

#include "sparkle_android_logger.h"
#include "were_thread.h"
#include "sparkle.h"
#include "sparkle_android.h"
#include "sparkle_audio.h"
#include "were_debug.h"

#include <csignal>
#include "were_timer.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define SOUND_THREAD

#ifdef SOUND_THREAD
#include <thread>
#endif


sparkle_service::~sparkle_service()
{
}

sparkle_service::sparkle_service(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance)
{
}

void sparkle_service::add_fd_listener(int fd, sparkle_service_fd_listener *listener)
{
    call_void_method("add_fd_listener", "(IJ)V", jint(fd), jlong(listener));
}

void sparkle_service::remove_fd_listener(int fd)
{
    call_void_method("remove_fd_listener", "(I)V", jint(fd));
}

void sparkle_service::add_idle_handler(sparkle_service_idle_handler *handler)
{
    call_void_method("add_idle_handler", "(J)V", jlong(handler));
}

void sparkle_service::remove_idle_handler(sparkle_service_idle_handler *handler)
{
    call_void_method("remove_idle_handler", "(J)V", jlong(handler));
}

int sparkle_service::display_width()
{
    return call_int_method("display_width", "()I");
}

int sparkle_service::display_height()
{
    return call_int_method("display_height", "()I");
}

class sparkle_native : public sparkle_service_fd_listener, public sparkle_service_idle_handler
{
public:

    virtual ~sparkle_native()
    {
        service_->remove_idle_handler(this);
        service_->remove_fd_listener(thread_->fd());
    }

    sparkle_native(JNIEnv *env, jobject instance)
    {
        thread_ = were_object_pointer<were_thread>(new were_thread());
        service_ = were_object_pointer<sparkle_service>(new sparkle_service(env, instance));

        sparkle_ = were_object_pointer<sparkle>(new sparkle());
        sparkle_android_ = were_object_pointer<sparkle_android>(new sparkle_android(sparkle_, service_));

#ifndef SOUND_THREAD
        audio_ = were_object_pointer<sparkle_audio>(new sparkle_audio());
#endif

        debug_ = were_object_pointer<were_debug>(new were_debug());

        /*
         * XXX1
         * No idea why we need dup() here. But if we don't do this, android will close fd when
         * we close MainActivity. Seems like its also possible to fix such behavior by storing ParcelFileDescritor
         * on java side.
         */
        service_->add_fd_listener(dup(thread_->fd()), this);
        service_->add_idle_handler(this);

#ifdef SOUND_THREAD
        sound_thread_c_ = std::thread(&sparkle_native::sound, this);
#endif
    }

    void event()
    {
        thread_->process(0);
    };

    void idle()
    {
        thread_->idle();
    }

#ifdef SOUND_THREAD
    void sound()
    {
        sound_thread_ = were_object_pointer<were_thread>(new were_thread());
        audio_ = were_object_pointer<sparkle_audio>(new sparkle_audio());
        sound_thread_->run();
    }
#endif

private:
    sparkle_android_logger logger_;
    were_object_pointer<were_thread> thread_;
    were_object_pointer<sparkle_service> service_;
    were_object_pointer<sparkle> sparkle_;
    were_object_pointer<sparkle_android> sparkle_android_;
    were_object_pointer<sparkle_audio> audio_;
    were_object_pointer<were_debug> debug_;
#ifdef SOUND_THREAD
    std::thread sound_thread_c_;
    were_object_pointer<were_thread> sound_thread_;
#endif
};


extern "C" JNIEXPORT jlong JNICALL
Java_com_sion_sparkle_SparkleService_native_1create(JNIEnv *env, jobject instance)
{
    sparkle_native *native = new sparkle_native(env, instance);

    return jlong(native);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    // XXX1
    raise(SIGINT); /* That is how we deal with program termination and proper resource deallocation! Yeah! */

    //sparkle_native *native__ = reinterpret_cast<sparkle_native *>(native);
    //delete native__;
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_fd_1event(JNIEnv *env, jobject instance, jlong user)
{
    sparkle_service_fd_listener *listener__ = reinterpret_cast<sparkle_service_fd_listener *>(user);
    listener__->event();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_idle_1event(JNIEnv *env, jobject instance, jlong user)
{
    sparkle_service_idle_handler *handler__ = reinterpret_cast<sparkle_service_idle_handler *>(user);
    handler__->idle();
}
