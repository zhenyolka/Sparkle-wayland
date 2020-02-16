#include "were_android_application.h"
#include "were_exception.h"
#include "were_backtrace.h"
#include "sparkle_android_logger.h"
#include "were_debug.h"
#include "were_registry.h"
#include "sparkle_settings.h"
#include <unistd.h> // dup()
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/stat.h> // chmod()
//#include <csignal> // SIGINT

static bool created = false;

were_android_application::~were_android_application()
{
    fprintf(stdout, "~were_android_application\n");
}

were_android_application::were_android_application(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance)
{
    auto this_wop = make_wop(this);

    fprintf(stdout, "were_android_application\n");

    files_dir_ = call_string_method("files_dir", "()Ljava/lang/String;");
    home_dir_ = call_string_method("home_dir", "()Ljava/lang/String;");


    sparkle_android_logger *logger = new sparkle_android_logger();
    logger->redirect_output(files_dir_ + "/log.txt");

    setup();


    were_object_pointer<sparkle_settings> settings(new sparkle_settings());
    settings->link(this_wop);
    settings->load(files_dir_ + "/sparkle.config");
    global_set<sparkle_settings>(settings);
    were_object::connect(settings, &were_object::destroyed, settings, []()
    {
        global_clear<sparkle_settings>();
    });
    //XXX1 Move to ptr
}

void were_android_application::enable_native_loop(int fd)
{
    call_void_method("enable_native_loop", "(I)V", jint(fd));
}

void were_android_application::disable_native_loop()
{
    call_void_method("disable_native_loop", "()V");
}

void were_android_application::copy_asset(AAssetManager *assets, const char *source, const char *destination, mode_t mode)
{
    std::string out__ = files_dir_ + "/" + destination;

    if(::access(out__.c_str(), F_OK) != -1)
        return;

    AAsset *asset = AAssetManager_open(assets, source, AASSET_MODE_STREAMING);
    if (asset == nullptr)
        throw were_exception(WE_SIMPLE);

    char buffer[BUFSIZ];
    int n = 0;

    FILE *out = fopen(out__.c_str(), "w");
    if (out == nullptr)
        throw were_exception(WE_SIMPLE);

    while ((n = AAsset_read(asset, buffer, BUFSIZ)) > 0)
        fwrite(buffer, n, 1, out);

    fclose(out);

    AAsset_close(asset);

    if (chmod(out__.c_str(), mode) == -1)
        throw were_exception(WE_SIMPLE);
}

void were_android_application::setup()
{
    if (chmod(home_dir_.c_str(), 0755) == -1)
        throw were_exception(WE_SIMPLE);
    if (chmod(files_dir_.c_str(), 0755) == -1)
        throw were_exception(WE_SIMPLE);


    jobject java_assets = call_object_method("getAssets", "()Landroid/content/res/AssetManager;");

    AAssetManager *assets = AAssetManager_fromJava(env(), java_assets);
    if (assets == nullptr)
        throw were_exception(WE_SIMPLE);

    copy_asset(assets, "sparkle.config", "sparkle.config", 0644);
    copy_asset(assets, "sparkle.sh", "sparkle.sh", 0644);
    copy_asset(assets, "user.sh", "user.sh", 0644);

    env()->DeleteLocalRef(java_assets);
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_sion_sparkle_WereApplication_native_1create(JNIEnv *env, jobject instance)
{
    if (created)
        throw were_exception(WE_SIMPLE);
    created = true;


    were_backtrace *backtrace = new were_backtrace();
    backtrace->enable();

    were_debug *debug = new were_debug();
    were_registry<were_debug *>::set(debug);
    debug->start();

    t_l_global_set<were_thread>(were_object_pointer<were_thread>(new were_thread()));

    were_object_pointer<were_android_application> native__(new were_android_application(env, instance));
    native__.increment_reference_count();

    native__->enable_native_loop(dup(t_l_global<were_thread>()->fd()));
    t_l_global<were_thread>()->process_queue();

    global_set<were_android_application>(native__);

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
