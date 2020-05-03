#include "sparkle_service.h"
#include "sparkle_view.h"
#include "sparkle.h"
#include "sparkle_audio.h"
#include "sparkle_global.h"
#include "sparkle_shell.h"
#include "sparkle_settings.h"
#include "were_android_application.h"



sparkle_service::~sparkle_service()
{
    fprintf(stdout, "~sparkle_service\n");
}

sparkle_service::sparkle_service(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance),
    sparkle_(new sparkle(global<were_android_application>()->files_dir()))
{
    fprintf(stdout, "sparkle_service\n");

    auto this_wop = were_pointer(this);

    were::link(sparkle_, this_wop);

    sparkle_->set_size(display_width(), display_height());

    were_pointer<sparkle_audio> sparkle_audio__(new sparkle_audio(global<were_android_application>()->files_dir() + "/audio-0"));
    were::link(sparkle_audio__, this_wop);

    were::connect(sparkle_->shell(), &sparkle_global<sparkle_shell>::instance, this_wop, [this_wop](were_pointer<sparkle_shell> shell)
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

void sparkle_service::register_producer(were_pointer<were_surface_producer> producer)
{
    auto this_wop = were_pointer(this);

    were::connect(producer, &were_surface_producer::surface_created, this_wop, [this_wop](were_pointer<were_surface> surface)
    {
        were_pointer<sparkle_view> view(new sparkle_view(env(), this_wop, surface));
        were::link(view, surface);
    });
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_sion_sparkle_SparkleService_native_1create(JNIEnv *env, jobject instance)
{
    were_pointer<sparkle_service> native__(new sparkle_service(env, instance));
    native__.increment_reference_count();
    return jlong(native__.access());
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    were_pointer<sparkle_service> native__(reinterpret_cast<sparkle_service *>(native));
    native__.decrement_reference_count();
    native__->collapse();
}
