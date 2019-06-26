#include "sparkle_service.h"

#include "sparkle_android_logger.h"
#include "were_thread.h"
#include "sparkle.h"
#include "sparkle_android.h"
#include "were_debug.h"



#include <android/log.h>



sparkle_service::~sparkle_service()
{
    __android_log_print(ANDROID_LOG_INFO, "Sparkle", "222");
}

sparkle_service::sparkle_service(JNIEnv *env, jobject instance) :
    sparkle_java_object(env, instance)
{
    __android_log_print(ANDROID_LOG_INFO, "Sparkle", "111");
}

void sparkle_service::add_fd_listener(int fd, sparkle_service_fd_listener *listener)
{
    jmethodID id = env()->GetMethodID(class1(), "add_fd_listener", "(IJ)V");
    env()->CallVoidMethod(object1(), id, jint(fd), jlong(listener));
}

void sparkle_service::remove_fd_listener(int fd)
{
    jmethodID id = env()->GetMethodID(class1(), "remove_fd_listener", "(I)V");
    env()->CallVoidMethod(object1(), id, jint(fd));
}

class sparkle_native : public sparkle_service_fd_listener
{
public:

    virtual ~sparkle_native()
    {
        service_->remove_fd_listener(thread_->fd());
    }

    sparkle_native(JNIEnv *env, jobject instance)
    {
        thread_ = were_object_pointer<were_thread>(new were_thread());
        service_ = were_object_pointer<sparkle_service>(new sparkle_service(env, instance));

        sparkle_ = were_object_pointer<sparkle>(new sparkle());
        sparkle_android_ = were_object_pointer<sparkle_android>(new sparkle_android(sparkle_));
        debug_ = were_object_pointer<were_debug>(new were_debug());

        service_->add_fd_listener(thread_->fd(), this);
    }

    void event()
    {
        thread_->process(0);
    };

private:
    sparkle_android_logger logger_;
    were_object_pointer<were_thread> thread_;
    were_object_pointer<sparkle_service> service_;
    were_object_pointer<sparkle> sparkle_;
    were_object_pointer<sparkle_android> sparkle_android_;
    were_object_pointer<were_debug> debug_;
};


extern "C" JNIEXPORT jlong JNICALL
Java_com_sion_sparkle_SparkleService_native_1create(JNIEnv *env, jobject instance)
{
    sparkle_native *native = new sparkle_native(env, instance);

    return jlong(native);
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_native_1destroy(JNIEnv *env, jobject instance, jlong native)
{
    sparkle_native *native__ = reinterpret_cast<sparkle_native *>(native);
    delete native__;
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_fd_1event(JNIEnv *env, jobject instance, jlong listener)
{
    sparkle_service_fd_listener *listener__ = reinterpret_cast<sparkle_service_fd_listener *>(listener);
    listener__->event();
}
