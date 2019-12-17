#include "sparkle_service.h"
#include "sparkle_view.h"
#include "sparkle_android_logger.h"
#include "were_thread.h"
#include "sparkle.h"
#include "sparkle_audio.h"
#include "sparkle_global.h"
#include "sparkle_shell.h"
#include "sparkle_seat.h"
#include "sparkle_output.h"
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

    were_object_pointer<sparkle_audio> sparkle_audio__(new sparkle_audio(files_dir_ + "/audio-0"));
    sparkle_audio__->link(this_wop);


    were_object::connect(sparkle_->shell(), &sparkle_global<sparkle_shell>::instance, this_wop, [this_wop](were_object_pointer<sparkle_shell> shell)
    {
        this_wop->register_producer(shell);

        were_object::connect(shell, &sparkle_shell::shell_surface_created, this_wop, [this_wop](were_object_pointer<sparkle_shell_surface> shell_surface, were_object_pointer<sparkle_surface> surface)
        {
            were_object::connect(this_wop, &sparkle_service::keyboard_created, surface, [surface](were_object_pointer<sparkle_keyboard> keyboard)
            {
                surface->register_keyboard(keyboard);
            });

            were_object::connect(this_wop, &sparkle_service::pointer_created, surface, [surface](were_object_pointer<sparkle_pointer> pointer)
            {
                surface->register_pointer(pointer);
            });

            were_object::connect(this_wop, &sparkle_service::touch_created, surface, [surface](were_object_pointer<sparkle_touch> touch)
            {
                surface->register_touch(touch);
            });

            were_object::emit(this_wop, &sparkle_service::surface_created, surface);
        });
    });

    were_object::connect(sparkle_->seat(), &sparkle_global<sparkle_seat>::instance, this_wop, [this_wop](were_object_pointer<sparkle_seat> seat)
    {
        were_object::connect(seat, &sparkle_seat::keyboard_created, this_wop, [this_wop](were_object_pointer<sparkle_keyboard> keyboard)
        {
            were_object::connect(this_wop, &sparkle_service::surface_created, keyboard, [keyboard](were_object_pointer<sparkle_surface> surface)
            {
                surface->register_keyboard(keyboard);
            });

            were_object::emit(this_wop, &sparkle_service::keyboard_created, keyboard);
        });

        were_object::connect(seat, &sparkle_seat::pointer_created, this_wop, [this_wop](were_object_pointer<sparkle_pointer> pointer)
        {
            were_object::connect(this_wop, &sparkle_service::surface_created, pointer, [pointer](were_object_pointer<sparkle_surface> surface)
            {
                surface->register_pointer(pointer);
            });

            were_object::emit(this_wop, &sparkle_service::pointer_created, pointer);
        });

        were_object::connect(seat, &sparkle_seat::touch_created, this_wop, [this_wop](were_object_pointer<sparkle_touch> touch)
        {
            were_object::connect(this_wop, &sparkle_service::surface_created, touch, [touch](were_object_pointer<sparkle_surface> surface)
            {
                surface->register_touch(touch);
            });

            were_object::emit(this_wop, &sparkle_service::touch_created, touch);
        });
    });

    were_object::connect(sparkle_->output(), &sparkle_global<sparkle_output>::instance, this_wop, [this_wop](were_object_pointer<sparkle_output> output)
    {
        int width = this_wop->display_width();
        int height = this_wop->display_height();
        int dpi = this_wop->sparkle_->settings()->get_int("DPI", 96);
        int mm_width = width * 254 / (dpi * 10);
        int mm_height = height * 254 / (dpi * 10);

        fprintf(stdout, "display size: %dx%d %dx%d\n", width, height, mm_width, mm_height);

        output->send_geometry(0, 0, mm_width, mm_height, 0, "Barely working solutions", "Sparkle", 0);

        if (output->version() >= WL_OUTPUT_SCALE_SINCE_VERSION)
            output->send_scale(1);

        output->send_mode(WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED, width, height, 60000);

        if (output->version() >= WL_OUTPUT_DONE_SINCE_VERSION)
            output->send_done();
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
