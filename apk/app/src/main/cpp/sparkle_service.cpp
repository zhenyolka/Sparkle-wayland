#include "sparkle_service.h"

#include "sparkle_android_logger.h"
#include "were_thread.h"
#include "sparkle.h"
#include "sparkle_android.h"
#include "were_debug.h"

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

int sparkle_service::display_width()
{
    jmethodID id = env()->GetMethodID(class1(), "display_width", "()I");
    int x = env()->CallIntMethod(object1(), id);
    return x;
}

int sparkle_service::display_height()
{
    jmethodID id = env()->GetMethodID(class1(), "display_height", "()I");
    int x = env()->CallIntMethod(object1(), id);
    return x;
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
        sparkle_android_ = were_object_pointer<sparkle_android>(new sparkle_android(sparkle_, service_));
        debug_ = were_object_pointer<were_debug>(new were_debug());

        /*
         * XXX
         * No idea why we need dup() here. But if we don't do this, android will close fd when
         * we close MainActivity. Seems like its also possible to fix such behavior by storing ParcelFileDescritor
         * on java side.
         */
        service_->add_fd_listener(dup(thread_->fd()), this);
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
    raise(SIGINT); /* That is how we deal with program termination and proper resource deallocation! Yeah! */

    //sparkle_native *native__ = reinterpret_cast<sparkle_native *>(native);
    //delete native__;
}

extern "C" JNIEXPORT void JNICALL
Java_com_sion_sparkle_SparkleService_fd_1event(JNIEnv *env, jobject instance, jlong listener)
{
    sparkle_service_fd_listener *listener__ = reinterpret_cast<sparkle_service_fd_listener *>(listener);
    listener__->event();
}
