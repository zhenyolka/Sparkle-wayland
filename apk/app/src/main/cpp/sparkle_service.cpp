#include "sparkle_service.h"


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




sparkle_service::~sparkle_service()
{
    remove_idle_handler(this);
    remove_fd_listener(thread_->fd());
}

sparkle_service::sparkle_service(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance)
{
    MAKE_THIS_WOP

    thread_ = were_object_pointer<were_thread>(new were_thread());

    sparkle_ = were_object_pointer<sparkle>(new sparkle());
    sparkle_android_ = were_object_pointer<sparkle_android>(new sparkle_android(sparkle_, this_wop));

#ifndef SOUND_THREAD
    audio_ = were_object_pointer<sparkle_audio>(new sparkle_audio());
#endif

    debug_ = were_object_pointer<were_debug>(new were_debug());

    add_fd_listener(thread_->fd(), this);
    add_idle_handler(this);

#ifdef SOUND_THREAD
    sound_thread_c_ = std::thread(&sparkle_service::sound, this);
#endif
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

void sparkle_service::event()
{
    thread_->process(0);
}

void sparkle_service::idle()
{
    thread_->idle();
}


#ifdef SOUND_THREAD
void sparkle_service::sound()
{
    sound_thread_ = were_object_pointer<were_thread>(new were_thread());
    audio_ = were_object_pointer<sparkle_audio>(new sparkle_audio());
    sound_thread_->run();
}
#endif


extern "C" JNIEXPORT jlong JNICALL
Java_com_sion_sparkle_SparkleService_native_1create(JNIEnv *env, jobject instance)
{
    were_object_pointer<sparkle_service> native__(new sparkle_service(env, instance));
    native__->increment_reference_count();

    return jlong(native__.get());
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    // XXX1
    raise(SIGINT); /* That is how we deal with program termination and proper resource deallocation! Yeah! */

    //were_object_pointer<sparkle_service> native__(reinterpret_cast<sparkle_service *>(native));
    //native__->decrement_reference_count();
    //native__.collapse();
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
