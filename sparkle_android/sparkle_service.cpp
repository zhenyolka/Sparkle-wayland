#include "sparkle_service.h"
#include "sparkle_view.h"
#include "sparkle_android_logger.h"
#include "were_thread.h"
#include "sparkle.h"
#include "sparkle_audio.h"
#include "sparkle_global.h"
#include "sparkle_shell.h"
#include "sparkle_settings.h"
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

    sparkle_ = were_object_pointer<sparkle>(new sparkle(files_dir_));
    sparkle_->link(this_wop);

    sparkle_->set_size(display_width(), display_height());

    were_object_pointer<sparkle_audio> sparkle_audio__(new sparkle_audio(files_dir_ + "/audio-0"));
    sparkle_audio__->link(this_wop);

    were_object::connect(sparkle_->shell(), &sparkle_global<sparkle_shell>::instance, this_wop, [this_wop](were_object_pointer<sparkle_shell> shell)
    {
        this_wop->register_producer(shell);
    });
}

int sparkle_service::display_width() const
{
    return const_cast<sparkle_service *>(this)->call_int_method("display_width", "()I");
}

int sparkle_service::display_height() const
{
    return const_cast<sparkle_service *>(this)->call_int_method("display_height", "()I");
}

void sparkle_service::enable_native_loop(int fd)
{
    call_void_method("enable_native_loop", "(I)V", jint(fd));
}

void sparkle_service::disable_native_loop()
{
    call_void_method("disable_native_loop", "()V");
}

void sparkle_service::register_producer(were_object_pointer<were_surface_producer> producer)
{
    MAKE_THIS_WOP

    were_object::connect(producer, &were_surface_producer::surface_created, this_wop, [this_wop](were_object_pointer<were_surface> surface)
    {
        were_object_pointer<sparkle_view> view(new sparkle_view(env(), this_wop, surface));
        view->link(surface);

        view->set_fast(this_wop->sparkle_->settings()->get_bool("fast", false));
        view->set_no_damage(this_wop->sparkle_->settings()->get_bool("no_damage", false));
    });
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

    were_thread::current_thread()->process_queue(); // XXX2

    return jlong(native__.access());
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    were_object_pointer<sparkle_service> native__(reinterpret_cast<sparkle_service *>(native));
    native__->disable_native_loop();

    native__.collapse();

    were_thread::current_thread()->run_for(1000);

    if (were_thread::current_thread()->reference_count() == 1) // XXX1 ->
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
    were_thread::current_thread()->run_once();

}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1loop_1idle_1event(JNIEnv *env, jobject instance, jlong user)
{
    were_thread::current_thread()->process_idle();
}
