#include "were_android_application.h"
#include "were_exception.h"
#include "were_backtrace.h"
#include "sparkle_android_logger.h"
#include "were_debug.h"
#include "were_registry.h"
#include "sparkle_settings.h"
#include <unistd.h> // dup()
//#include <csignal> // SIGINT

static bool created = false;

were_android_application::~were_android_application()
{
    fprintf(stdout, "~were_android_application\n");
}

were_android_application::were_android_application(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance)
{
    fprintf(stdout, "were_android_application\n");

    if (created)
        throw were_exception(WE_SIMPLE);
    created = true;

    files_dir_ = call_string_method("files_dir", "()Ljava/lang/String;");
    home_dir_ = call_string_method("home_dir", "()Ljava/lang/String;");

    //XXX2 Delete

    were_backtrace *backtrace = new were_backtrace();
    backtrace->enable();

    sparkle_android_logger *logger = new sparkle_android_logger();
    logger->redirect_output(files_dir_ + "/log.txt");

    were_debug *debug = new were_debug();
    were_registry<were_debug *>::set(debug);
    debug->start();
}

void were_android_application::enable_native_loop(int fd)
{
    call_void_method("enable_native_loop", "(I)V", jint(fd));
}

void were_android_application::disable_native_loop()
{
    call_void_method("disable_native_loop", "()V");
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_sion_sparkle_WereApplication_native_1create(JNIEnv *env, jobject instance)
{
    were_t_l_registry<were_object_pointer<were_thread>>::set(
        were_object_pointer<were_thread>(new were_thread()));

    were_object_pointer<were_android_application> native__(new were_android_application(env, instance));
    native__.increment_reference_count();

    were_t_l_registry<were_object_pointer<sparkle_settings>>::set(
        were_object_pointer<sparkle_settings>(new sparkle_settings()));
    were_t_l_registry<were_object_pointer<sparkle_settings>>::get()->load(native__->files_dir() + "/sparkle.config");

    native__->enable_native_loop(dup(t_l_global<were_thread>()->fd()));
    t_l_global<were_thread>()->process_queue();

    t_l_global_set<were_android_application>(native__);

    return jlong(native__.access());
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_WereApplication_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    were_object_pointer<were_android_application> native__(reinterpret_cast<were_android_application *>(native));
    native__.decrement_reference_count();

    native__->disable_native_loop();
    //t_l_global<were_thread>()->run_for(1000);

    native__.collapse();

    //fprintf(stdout, "SIGINT\n");
    //raise(SIGINT); /* That is how we deal with program termination and proper resource deallocation! Yeah! */
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_WereApplication_native_1loop_1fd_1event(JNIEnv *env, jobject instance, jlong user)
{
    t_l_global<were_thread>()->run_once();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_WereApplication_native_1loop_1idle_1event(JNIEnv *env, jobject instance, jlong user)
{
    t_l_global<were_thread>()->process_idle();
}
