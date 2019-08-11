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



sparkle_service::~sparkle_service()
{
    //remove_idle_handler(this);
    //remove_fd_listener(thread_->fd());
}

sparkle_service::sparkle_service(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance)
{
    MAKE_THIS_WOP

    files_dir_ = call_string_method("files_dir", "()Ljava/lang/String;");
    sparkle_android_logger::redirect_output(files_dir_ + "/log.txt");

    thread_ = were_object_pointer<were_thread>(new were_thread());

    sparkle_ = were_object_pointer<sparkle>(new sparkle(files_dir_));
    sparkle_android_ = were_object_pointer<sparkle_android>(new sparkle_android(sparkle_, this_wop));

#ifndef SOUND_THREAD
    audio_ = were_object_pointer<sparkle_audio>(new sparkle_audio(files_dir_ + "/audio-0"));
#endif

    debug_ = were_object_pointer<were_debug>(new were_debug());

    add_fd_listener(thread_->fd(), this_wop);
    add_idle_handler(this_wop);

    were_object::connect_x(this_wop, this_wop, [this_wop]()
    {
        this_wop->remove_fd_listener(this_wop->thread_->fd(), this_wop);
        this_wop->remove_idle_handler(this_wop);
    });

#ifdef SOUND_THREAD
    sound_thread_c_ = std::thread(&sparkle_service::sound, this);
#endif
}

void sparkle_service::add_fd_listener(int fd, were_object_pointer<sparkle_service_fd_listener> listener)
{
    listener.increment_reference_count();
    call_void_method("add_fd_listener", "(IJ)V", jint(fd), jlong(listener.get()));
}

void sparkle_service::remove_fd_listener(int fd, were_object_pointer<sparkle_service_fd_listener> listener)
{
    call_void_method("remove_fd_listener", "(I)V", jint(fd));
    listener.decrement_reference_count();
}

void sparkle_service::add_idle_handler(were_object_pointer<sparkle_service_idle_handler> handler)
{
    handler.increment_reference_count();
    call_void_method("add_idle_handler", "(J)V", jlong(handler.get()));
}

void sparkle_service::remove_idle_handler(were_object_pointer<sparkle_service_idle_handler> handler)
{
    call_void_method("remove_idle_handler", "(J)V", jlong(handler.get()));
    handler.decrement_reference_count();
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
    audio_ = were_object_pointer<sparkle_audio>(new sparkle_audio(files_dir_ + "/audio-0"));
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
