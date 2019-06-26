#include "SparkleCompositorAndroid.h"
#include "SparkleCompositorSurfaceAndroid.h"
#include "generated/Sparkle_wl_surface.h"
#include "generated/Sparkle_wl_pointer.h"
#include "generated/Sparkle_wl_keyboard.h"
#include "generated/Sparkle_wl_touch.h"


SparkleCompositorAndroid::~SparkleCompositorAndroid()
{
}

SparkleCompositorAndroid::SparkleCompositorAndroid(Sparkle *sparkle, SparkleService *service) :
    sparkle_(sparkle), service_(service)
{
    pointerFocus_ = nullptr;
    keyboardFocus_ = nullptr;

    sparkle_->surface_created.connect([this](Sparkle_wl_surface *surface)
    {
        SparkleCompositorSurfaceAndroid *android_surface = new SparkleCompositorSurfaceAndroid(sparkle_, surface, service_);
        android_surface->link(surface, true);
        android_surface->link(this, true);

        surfaceCreated(android_surface);

        sparkle_->pointer_created.connect(static_cast<SparkleCompositorSurface *>(android_surface),
            &SparkleCompositorSurfaceAndroid::registerPointer)->link(android_surface, true);
        sparkle_->keyboard_created.connect(static_cast<SparkleCompositorSurface *>(android_surface),
            &SparkleCompositorSurfaceAndroid::registerKeyboard)->link(android_surface, true);
        sparkle_->touch_created.connect(static_cast<SparkleCompositorSurface *>(android_surface),
            &SparkleCompositorSurfaceAndroid::registerTouch)->link(android_surface, true);
        sparkle_->shell_surface_created.connect([android_surface](Sparkle_wl_surface *surface)
        {
            if (surface == android_surface->surface())
                android_surface->setEnabled(true);
        })->link(android_surface, true);




        android_surface->destroyed.connect([this, android_surface]()
        {
            if (pointerFocus_ == android_surface)
                pointerFocus_ = nullptr;
            if (keyboardFocus_ == android_surface)
                keyboardFocus_ = nullptr;
        })->link(this, true);


        android_surface->mousePointerMotion.connect([this, android_surface](int x, int y)
        {
            if (pointerFocus_ == android_surface)
                return;

            if (pointerFocus_ != nullptr)
                pointerFocus_->mousePointerLeave();

            pointerFocus_ = android_surface;
            pointerFocus_->mousePointerEnter();

        })->link(this, true);


        android_surface->mouseButtonPress.connect([this, android_surface](int button)
        {
            if (keyboardFocus_ == android_surface)
                return;

            if (keyboardFocus_ != nullptr)
                keyboardFocus_->keyboardLeave();

            keyboardFocus_ = android_surface;
            keyboardFocus_->keyboardEnter();

        })->link(this, true);


        android_surface->touchDown.connect([this, android_surface](int id, int x, int y)
        {
            if (keyboardFocus_ == android_surface)
                return;

            if (keyboardFocus_ != nullptr)
                keyboardFocus_->keyboardLeave();

            keyboardFocus_ = android_surface;
            keyboardFocus_->keyboardEnter();

        })->link(this, true);




    })->link(this, true);

    sparkle_->pointer_created.connect([this](Sparkle_wl_pointer *pointer)
    {
        surfaceCreated.connect([pointer](SparkleCompositorSurfaceAndroid *android_surface)
        {
            android_surface->registerPointer(pointer);
        })->link(pointer, true);
    })->link(this, true);

    sparkle_->keyboard_created.connect([this](Sparkle_wl_keyboard *keyboard)
    {
        surfaceCreated.connect([keyboard](SparkleCompositorSurfaceAndroid *android_surface)
        {
            android_surface->registerKeyboard(keyboard);
        })->link(keyboard, true);
    })->link(this, true);

    sparkle_->touch_created.connect([this](Sparkle_wl_touch *touch)
    {
        surfaceCreated.connect([touch](SparkleCompositorSurfaceAndroid *android_surface)
        {
            android_surface->registerTouch(touch);
        })->link(touch, true);
    })->link(this, true);
}
