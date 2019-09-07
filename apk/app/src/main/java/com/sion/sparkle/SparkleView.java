package com.sion.sparkle;

// Basic
import androidx.annotation.Keep;
import android.os.Build;
import android.content.Intent;
import android.content.Context;
import android.util.Log;

import android.view.SurfaceView;

import android.view.SurfaceHolder;
import android.view.Surface;

import android.view.WindowManager;
import android.view.Gravity;

// Input
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.InputDevice; // Check input source
import android.view.inputmethod.InputMethodManager; // Software keyboard

// Broadcast receiver
import android.content.BroadcastReceiver;
import android.content.IntentFilter;


// Orientation control
import android.util.DisplayMetrics;
import android.content.pm.ActivityInfo;

//import android.graphics.PixelFormat;


public class SparkleView extends SurfaceView implements SurfaceHolder.Callback
{
    @Keep
    SparkleView(SparkleService sparkle, long user)
    {
        super(sparkle);


        this.sparkle_ = sparkle;
        this.user = user;

        int windowType;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
            windowType = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
        else
            windowType = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT; // WindowManager.LayoutParams.TYPE_PHONE;

        // WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
        int flags = 0;
        flags |= WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL; // allow any pointer events outside of the window to be sent to the windows behind it
        flags |= WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON;
        flags |= WindowManager.LayoutParams.FLAG_DIM_BEHIND;


        params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.MATCH_PARENT, WindowManager.LayoutParams.MATCH_PARENT, 0, 0,
                windowType,
                flags,
                0);
        params.gravity = Gravity.CENTER;
        params.setTitle("Sparkle");


        setZOrderOnTop(true);
        setFocusableInTouchMode(true);
        getHolder().addCallback(this);
        //getHolder().setFormat(PixelFormat.TRANSLUCENT);
        //getHolder().setFormat(PixelFormat.OPAQUE);



        params.x = 0;
        params.y = 0;
        params.width = 100;
        params.height = 100;
        //params.alpha = 0.5F;
        //params.dimAmount = 1.0F;
        //params.format = PixelFormat.TRANSLUCENT;
        //params.format = PixelFormat.OPAQUE;


        setVisibility(VISIBLE);



        DisplayMetrics display_metrics = new DisplayMetrics();
        sparkle_.window_manager_.getDefaultDisplay().getMetrics(display_metrics);
        if (display_metrics.widthPixels > display_metrics.heightPixels)
            params.screenOrientation = ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE;
        else
            params.screenOrientation = ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT;


        enabled_ = false;
        rmb_ = false;
        rmb_down_ = false;


        receiver_ = new BroadcastReceiver()
        {
            @Override
            public void onReceive(Context context, Intent intent)
            {
                String action = intent.getAction();

                if (action.equals(sparkle_.ACTION_HIDE) || action.equals(Intent.ACTION_CLOSE_SYSTEM_DIALOGS))
                {
                    set_visible(false);
                }
                else if (action.equals(sparkle_.ACTION_SHOW))
                {
                    set_visible(true);
                }
            }
        };

    }

    @Keep
    public void set_enabled(boolean enabled)
    {
        if (enabled && !enabled_)
        {
            sparkle_.window_manager_.addView(this, this.params);

            IntentFilter filter = new IntentFilter();
            filter.addAction(sparkle_.ACTION_HIDE);
            filter.addAction(sparkle_.ACTION_SHOW);
            filter.addAction(Intent.ACTION_CLOSE_SYSTEM_DIALOGS);
            sparkle_.registerReceiver(receiver_, filter);

            enabled_ = true;
        }
        else if (!enabled && enabled_)
        {
            sparkle_.window_manager_.removeView(this);

            sparkle_.unregisterReceiver(receiver_);

            enabled_ = false;
        }
    }

    @Keep
    public void set_visible(boolean visible)
    {
        if (visible)
            setVisibility(VISIBLE);
        else
            setVisibility(INVISIBLE);
    }

    @Keep
    public void set_position(int x, int y)
    {
        params.x = x;
        params.y = y;
        if (enabled_)
            sparkle_.window_manager_.updateViewLayout(this, params);
    }

    @Keep
    public void set_size(int width, int height)
    {
        params.width = width;
        params.height = height;
        if (enabled_)
            sparkle_.window_manager_.updateViewLayout(this, params);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        if (!enabled_) {return;}
        surface_changed(user, holder.getSurface());
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        if (!enabled_) {return;}
        surface_changed(user, holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        if (!enabled_) {return;}
        surface_changed(user, null);
    }

    private static boolean hasSource(int sources, int source)
    {
        return (sources & source) == source;
    }

    public boolean onMotionEvent(MotionEvent event)
    {
        if (!enabled_) {return false;}

        int source = event.getSource();

        if (hasSource(source, InputDevice.SOURCE_TOUCHSCREEN))
        {
            switch (event.getAction())
            {
                case MotionEvent.ACTION_DOWN:
                    if (rmb_)
                    {
                        pointer_motion(user, event.getX(), event.getY());
                        pointer_button_down(user, 2);
                        rmb_down_ = true;
                    }
                    else
                    {
                        touch_down(user, 0, event.getX(), event.getY());
                    }
                    return true;
                case MotionEvent.ACTION_UP:
                    if (rmb_down_)
                    {
                        pointer_button_up(user, 2);
                        rmb_down_ = false;
                    }
                    else
                    {
                        touch_up(user, 0, event.getX(), event.getY());
                    }
                    return true;
                case MotionEvent.ACTION_HOVER_MOVE:
                    pointer_motion(user, event.getX(), event.getY());
                    return true;
                case MotionEvent.ACTION_MOVE:
                    touch_motion(user, 0, event.getX(), event.getY());
                    return true;
            }
        }
        else if (hasSource(source, InputDevice.SOURCE_STYLUS))
        {
            switch (event.getAction())
            {
                case MotionEvent.ACTION_DOWN:
                    if (rmb_ || (event.getButtonState() & MotionEvent.BUTTON_STYLUS_PRIMARY) == MotionEvent.BUTTON_STYLUS_PRIMARY)
                    {
                        pointer_motion(user, event.getX(), event.getY());
                        pointer_button_down(user, 2);
                        rmb_down_ = true;
                    }
                    else
                    {
                        pointer_motion(user, event.getX(), event.getY());
                        pointer_button_down(user, 1);
                    }
                    return true;
                case MotionEvent.ACTION_UP:
                    if (rmb_down_)
                    {
                        pointer_button_up(user, 2);
                        rmb_down_ = false;
                    }
                    else
                    {
                        pointer_button_up(user, 1);
                    }
                    return true;
                case MotionEvent.ACTION_HOVER_MOVE:
                    pointer_motion(user, event.getX(), event.getY());
                    return true;
                case MotionEvent.ACTION_MOVE:
                    pointer_motion(user, event.getX(), event.getY());
                    return true;
            }
        }
        else if (hasSource(source, InputDevice.SOURCE_MOUSE))
        {
            switch (event.getAction())
            {
                case MotionEvent.ACTION_BUTTON_PRESS:
                    pointer_button_down(user, event.getActionButton());
                    return true;
                case MotionEvent.ACTION_BUTTON_RELEASE:
                    pointer_button_up(user, event.getActionButton());
                    return true;
                case MotionEvent.ACTION_HOVER_MOVE:
                    pointer_motion(user, event.getX(), event.getY());
                    return true;
                case MotionEvent.ACTION_MOVE:
                    pointer_motion(user, event.getX(), event.getY());
                    return true;
                case MotionEvent.ACTION_SCROLL:
                    if (event.getAxisValue(MotionEvent.AXIS_VSCROLL) > 0.0f)
                    {
                        pointer_button_down(user, 5);
                        pointer_button_up(user, 5);
                    }
                    else
                    {
                        pointer_button_down(user, 6);
                        pointer_button_up(user, 6);
                    }
                    return true;
            }
        }

        return false;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if (onMotionEvent(event))
            return true;
        else
            return super.onTouchEvent(event);
    }

    @Override
    public boolean onGenericMotionEvent(MotionEvent event)
    {
        if (onMotionEvent(event))
            return true;
        else
            return super.onGenericMotionEvent(event);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (!enabled_) {return false;}

        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            int source = event.getSource();
            if (hasSource(source, InputDevice.SOURCE_MOUSE))
            {
                pointer_button_down(user, 2);
                return true;
            }

            set_visible(false);
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_UP)
        {
            InputMethodManager imm = (InputMethodManager)sparkle_.getSystemService(sparkle_.INPUT_METHOD_SERVICE);
            //imm.showSoftInput(this, InputMethodManager.SHOW_IMPLICIT);
            imm.toggleSoftInput(0, 0);
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
        {
            rmb_ = true;
        }

        key_down(user, keyCode);

        return true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        if (!enabled_) {return false;}

        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            int source = event.getSource();
            if (hasSource(source, InputDevice.SOURCE_MOUSE))
            {
                pointer_button_up(user, 2);
                return true;
            }
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_UP)
        {
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
        {
            rmb_ = false;
        }

        key_up(user, keyCode);

        return true;
    }


    public native void surface_changed(long user, Surface surface);

    public native void key_down(long user, int code);
    public native void key_up(long user, int code);

    public native void pointer_button_down(long user, int button);
    public native void pointer_button_up(long user, int button);
    public native void pointer_motion(long user, float x, float y);
    public native void pointer_enter(long user);
    public native void pointer_leave(long user);

    public native void touch_down(long user, int id, float x, float y);
    public native void touch_up(long user, int id, float x, float y);
    public native void touch_motion(long user, int id, float x, float y);


    SparkleService sparkle_;
    long user = 0;
    WindowManager.LayoutParams params;
    boolean enabled_;
    boolean rmb_;
    boolean rmb_down_;
    BroadcastReceiver receiver_;
}
