#include "SparkleService.h"
#include "SparkleView.h"
#include "Sparkle.h"
#include "SparkleCompositorAndroid.h"

#include <signal.h>

SparkleService::~SparkleService()
{
    unsetNativeFd();

    delete compositor_;
    delete sparkle_;
}

SparkleService::SparkleService(JNIEnv *env, jobject instance) :
    SparkleJavaObject(env, instance)
{
    sparkle_ = new Sparkle();
    compositor_ = new SparkleCompositorAndroid(sparkle_, this);

    setNativeFd(sparkle_->fd());
    nativeEvent.connect(sparkle_, &Sparkle::process);
    displaySize.connect(sparkle_, &Sparkle::setDisplaySize);
}

void SparkleService::setNativeFd(int fd)
{
    jmethodID id = env()->GetMethodID(class_(), "setNativeFd", "(I)V");
    env()->CallVoidMethod(object_(), id, jint(fd));
}

void SparkleService::unsetNativeFd()
{
    jmethodID id = env()->GetMethodID(class_(), "unsetNativeFd", "()V");
    env()->CallVoidMethod(object_(), id);
}

SparkleView *SparkleService::createView()
{
    jmethodID id = env()->GetMethodID(class_(), "createView", "()Lcom/sion/sparkle/SparkleView;");
    jobject instance = env()->CallObjectMethod(object_(), id);

    SparkleView *view = new SparkleView(env(), instance);

    return view;
}

void SparkleService::destroyView(SparkleView *view)
{
    jmethodID id = env()->GetMethodID(class_(), "destroyView", "(Lcom/sion/sparkle/SparkleView;)V");
    env()->CallVoidMethod(object_(), id, view->object_());

    delete view;
}

/* ================================================================================================================== */

extern "C" JNIEXPORT jlong JNICALL
Java_com_sion_sparkle_SparkleService_native_1create(JNIEnv *env, jobject instance)
{
    SparkleService *service = new SparkleService(env, instance);

    return jlong(service);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1destroy(JNIEnv *env, jobject instance, jlong user)
{
    SparkleService *service = reinterpret_cast<SparkleService *>(user);
    delete service;

    raise(SIGINT); // XXX
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1event(JNIEnv *env, jobject instance, jlong user)
{
    SparkleService *service = reinterpret_cast<SparkleService *>(user);
    service->nativeEvent();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_display_1size(JNIEnv *env, jobject instance, jlong user, jint width, jint height)
{
    SparkleService *service = reinterpret_cast<SparkleService *>(user);
    service->displaySize(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_show_1all(JNIEnv *env, jobject instance, jlong user)
{
    SparkleService *service = reinterpret_cast<SparkleService *>(user);
    service->showAll();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_hide_1all(JNIEnv *env, jobject instance, jlong user)
{
    SparkleService *service = reinterpret_cast<SparkleService *>(user);
    service->hideAll();
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_stop_1server(JNIEnv *env, jobject instance, jlong user)
{
    SparkleService *service = reinterpret_cast<SparkleService *>(user);
    service->stopServer();
}

#if 0
void *func(void *arg)
{
    JNIEnv *env;
    jint res;

    res = javaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (res != JNI_OK)
    {
        res = javaVM->AttachCurrentThread(&env, NULL);
        if (res != JNI_OK)
            return NULL;
    }

    javaVM->DetachCurrentThread();

    return NULL;
}
#endif
