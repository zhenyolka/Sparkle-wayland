#include "sparkle_service.h"

#include "sparkle_android_logger.h"
#include "were_thread.h"
#include "sparkle.h"
#include "sparkle_android.h"
#include "sparkle_audio.h"
#include "were_debug.h"
#include "were_backtrace.h"
#include <unistd.h> // dup()
//#include <csignal> // SIGINT


sparkle_service::~sparkle_service()
{
}

sparkle_service::sparkle_service(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance)
{
    MAKE_THIS_WOP

    files_dir_ = call_string_method("files_dir", "()Ljava/lang/String;");

    were_object_pointer<sparkle> sparkle__ = were_object_pointer<sparkle>(new sparkle(files_dir_));
    sparkle__->add_dependency(this_wop);

    were_object_pointer<sparkle_android> sparkle_android__(new sparkle_android(sparkle__, this_wop));
    sparkle_android__->add_dependency(this_wop);

    were_object_pointer<sparkle_audio> sparkle_audio__(new sparkle_audio(files_dir_ + "/audio-0"));
    sparkle_audio__->add_dependency(this_wop);
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

extern "C" JNIEXPORT jlong JNICALL
Java_com_sion_sparkle_SparkleService_native_1create(JNIEnv *env, jobject instance)
{
    were_backtrace::instance().enable();
    were_debug::instance().start();

    if (!were_thread::current_thread())
        were_object_pointer<were_thread> thread(new were_thread());

    were_object_pointer<sparkle_service> native__(new sparkle_service(env, instance));
    sparkle_android_logger::instance().redirect_output(native__->files_dir() + "/log.txt");
    native__->enable_native_loop(dup(were_thread::current_thread()->fd()));
    were_thread::current_thread()->process(0); // XXX2

    return jlong(native__.access());
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    were_object_pointer<sparkle_service> native__(reinterpret_cast<sparkle_service *>(native));
    native__->disable_native_loop();

    native__.collapse();

    for (int i = 0; i < 100; ++i) // XXX2 Clean
        were_thread::current_thread()->process(10);

    if (were_thread::current_thread().reference_count() == 1)
    {
        were_thread::current_thread().collapse();
        fprintf(stdout, "thread collapsed\n");
        were_debug::instance().stop();
    }

    //fprintf(stdout, "SIGINT\n");
    //raise(SIGINT); /* That is how we deal with program termination and proper resource deallocation! Yeah! */
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
