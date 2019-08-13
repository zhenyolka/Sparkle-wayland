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
}

sparkle_service::sparkle_service(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance)
{
    MAKE_THIS_WOP

    files_dir_ = call_string_method("files_dir", "()Ljava/lang/String;");
    sparkle_android_logger::redirect_output(files_dir_ + "/log.txt");

    sparkle_ = were_object_pointer<sparkle>(new sparkle(files_dir_));
    sparkle_->add_dependency(this_wop);

    were_object_pointer<sparkle_android> sparkle_android__(new sparkle_android(sparkle_, this_wop));
    sparkle_android__->add_dependency(this_wop);

#ifndef SOUND_THREAD
    audio_ = were_object_pointer<sparkle_audio>(new sparkle_audio(files_dir_ + "/audio-0"));
    audio_->add_dependency(this_wop);
#endif

#ifdef SOUND_THREAD
    sound_thread_c_ = std::thread(&sparkle_service::sound, this);
    sound_thread_c_.detach(); // XXX1
#endif
}

void sparkle_service::enable_native_loop(int fd)
{
    call_void_method("enable_native_loop", "(I)V", jint(fd));
}

void sparkle_service::disable_native_loop()
{
    call_void_method("disable_native_loop", "()V");
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

    were_debug *debug = new were_debug(); // XXX1 stop
    debug->start();

    were_object_pointer<were_thread> thread(new were_thread());

    were_object_pointer<sparkle_service> native__(new sparkle_service(env, instance));
    native__->enable_native_loop(dup(thread->fd()));

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
    native__->disable_native_loop();


    native__.collapse();

    for (int i = 0; i < 100; ++i)
        were_thread::current_thread()->process(10);

    int x = were_thread::current_thread().reference_count();
    fprintf(stdout, "rc %d\n", x);

    fprintf(stdout, "SIGINT\n");
    raise(SIGINT);
#endif
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1loop_1fd_1event(JNIEnv *env, jobject instance, jlong user)
{
    were_thread::current_thread()->process(0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1loop_1idle_1event(JNIEnv *env, jobject instance, jlong user)
{
    were_thread::current_thread()->idle();
}
