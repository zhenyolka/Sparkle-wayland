#include "sparkle_service.h"

#include "sparkle_android_logger.h"
#include "were_thread.h"
#include "sparkle.h"
#include "sparkle_android.h"
#include "sparkle_audio.h"
#include "were_debug.h"
#include "were_backtrace.h"

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

    sparkle_ = were_object_pointer<sparkle>(new sparkle(files_dir_));
    were_object_pointer<sparkle_android> sparkle_android__(new sparkle_android(sparkle_, this_wop));
    sparkle_android__->add_dependency(this_wop);

#ifndef SOUND_THREAD
    audio_ = were_object_pointer<sparkle_audio>(new sparkle_audio(files_dir_ + "/audio-0"));
#endif

    //int fd = were_thread::current_thread()->fd();

    //add_fd_listener(fd, this_wop);
    //add_idle_handler(this_wop);

#if 0
    were_object::connect_x(this_wop, this_wop, [this_wop, fd]()
    {
        this_wop->remove_fd_listener(fd, this_wop);
        this_wop->remove_idle_handler(this_wop);
    });
#endif

#ifdef SOUND_THREAD
    sound_thread_c_ = std::thread(&sparkle_service::sound, this);
    sound_thread_c_.detach(); // XXX1
#endif
}

void sparkle_service::add_fd_listener(int fd)
{
    call_void_method("add_fd_listener", "(IJ)V", jint(fd), jlong(nullptr));
}

void sparkle_service::add_idle_handler()
{
    call_void_method("add_idle_handler", "(J)V", jlong(nullptr));
}

int sparkle_service::display_width()
{
    return call_int_method("display_width", "()I");
}

int sparkle_service::display_height()
{
    return call_int_method("display_height", "()I");
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
    were_backtrace::enable();

    were_debug *debug = new were_debug();
    debug->start();

    were_object_pointer<were_thread> thread(new were_thread());

    were_object_pointer<sparkle_service> native__(new sparkle_service(env, instance));
    native__->add_fd_listener(thread->fd());
    native__->add_idle_handler();

    return jlong(native__.get());
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
#if 0
    // XXX1
    raise(SIGINT); /* That is how we deal with program termination and proper resource deallocation! Yeah! */
#else
    were_object_pointer<sparkle_service> native__(reinterpret_cast<sparkle_service *>(native));

    native__.collapse();

    for (int i = 0; i < 100; ++i)
        were_thread::current_thread()->process(10);

    raise(SIGINT);
#endif
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_fd_1event(JNIEnv *env, jobject instance, jlong user)
{
#if 0
    sparkle_service_fd_listener *listener__ = reinterpret_cast<sparkle_service_fd_listener *>(user);
    listener__->event();
#endif
    // XXX1
    were_thread::current_thread()->process(0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_idle_1event(JNIEnv *env, jobject instance, jlong user)
{
#if 0
    sparkle_service_idle_handler *handler__ = reinterpret_cast<sparkle_service_idle_handler *>(user);
    handler__->idle();
#endif
    // XXX1
    were_thread::current_thread()->idle();
}
