package com.sion.sparkle;

import android.view.SurfaceView;

import android.view.SurfaceHolder;
import android.view.Surface;

import android.view.WindowManager;
import android.view.Gravity;

// Check version
import android.os.Build;

import android.view.MotionEvent;
import android.view.KeyEvent;

// Check input source
import android.view.InputDevice;

import android.util.Log;

// Software keyboard
import android.view.inputmethod.InputMethodManager;


public class SparkleView extends SurfaceView implements SurfaceHolder.Callback
{
    SparkleView(SparkleService sparkle, long user)
    {
        super(sparkle);

        Log.i("Sparkle", "Constructing view...");

        this.sparkle_ = sparkle;
        this.user = user;

        int windowType;
        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
            windowType = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
        else
            windowType = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT; // WindowManager.LayoutParams.TYPE_PHONE;

        params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.MATCH_PARENT, WindowManager.LayoutParams.MATCH_PARENT, 0, 0,
                windowType,
                WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL, // WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                0);
        params.gravity = Gravity.CENTER;
        params.setTitle("Sparkle");

        //setVisibility(INVISIBLE);
        setFocusableInTouchMode(true);
        getHolder().addCallback(this);


        params.x = 0;
        params.y = 0;
        params.width = 640;
        params.height = 480;

        setVisibility(VISIBLE);
    }

    public void set_enabled(boolean enabled)
    {
        if (enabled)
            sparkle_.window_manager_.addView(this, this.params);
        else
            sparkle_.window_manager_.removeView(this);
    }

    public void set_visible(boolean visible)
    {
        if (visible)
            setVisibility(VISIBLE);
        else
            setVisibility(INVISIBLE);
    }

    public void set_position(int x, int y)
    {
        params.x = x;
        params.y = y;
        sparkle_.window_manager_.updateViewLayout(this, params);
    }

    public void set_size(int width, int height)
    {
        params.width = width;
        params.height = height;
        sparkle_.window_manager_.updateViewLayout(this, params);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        Log.i("Sparkle", "Surface changed");
        if (user == 0) {return;}
        surface_changed(user, holder.getSurface());
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        Log.i("Sparkle", "Surface created");
        if (user == 0) {return;}
        surface_changed(user, holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        Log.i("Sparkle", "Surface destroyed");
        if (user == 0) {return;}
        surface_changed(user, null);
    }

    public static boolean hasSource(int sources, int source)
    {
        return (sources & source) == source;
    }

    public boolean onMotionEvent(MotionEvent event)
    {
        if (user == 0) {return false;}

        int source = event.getSource();

        if (hasSource(source, InputDevice.SOURCE_TOUCHSCREEN))
        {
            switch (event.getAction())
            {
                case MotionEvent.ACTION_DOWN:
                    touch_down(user, 0, event.getX(), event.getY());
                    return true;
                case MotionEvent.ACTION_UP:
                    touch_up(user, 0, event.getX(), event.getY());
                    return true;
                case MotionEvent.ACTION_MOVE:
                    touch_motion(user, 0, event.getX(), event.getY());
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
        if (user == 0) {return false;}

        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            set_visible(false); // XXX
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_UP)
        {
            InputMethodManager imm = (InputMethodManager)sparkle_.getSystemService(sparkle_.INPUT_METHOD_SERVICE);
            imm.showSoftInput(this, InputMethodManager.SHOW_IMPLICIT); // XXX Move
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
        {
        }

        key_down(user, keyCode);

        return true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        if (user == 0) {return false;}

        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_UP)
        {
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
        {
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
}
