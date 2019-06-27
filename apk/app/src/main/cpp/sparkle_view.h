#ifndef SPARKLE_VIEW_H
#define SPARKLE_VIEW_H

#include "were_object_2.h"
#include "sparkle_java_object.h"
#include "sparkle_service.h"

class ANativeWindow;

class sparkle_view : public were_object_2, public sparkle_java_object
{
public:
    ~sparkle_view();
    sparkle_view(JNIEnv *env, were_object_pointer<sparkle_service> service);

signals:
    were_signal<void (ANativeWindow *window)> surface_changed;
};

#endif // SPARKLE_VIEW_H
