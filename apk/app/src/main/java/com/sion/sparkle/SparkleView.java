package com.sion.sparkle;

import android.view.SurfaceView;
//import android.content.Context;
import android.view.SurfaceHolder;
//import android.view.Surface;

import android.view.WindowManager;
import android.view.Gravity;

// Check version
import android.os.Build;

import android.view.MotionEvent;
import android.view.KeyEvent;

// Check input source
import android.view.InputDevice;


public class SparkleView extends SurfaceView implements SurfaceHolder.Callback
{
    SparkleView(SparkleService sparkle, long user)
    {
        super(sparkle);

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

        setVisibility(INVISIBLE);
        setFocusableInTouchMode(true);
        getHolder().addCallback(this);
    }

    public void setVisible(boolean visible)
    {
        if (visible)
            setVisibility(VISIBLE);
        else
            setVisibility(INVISIBLE);
    }

    public void resize(int width, int height)
    {
        params.width = width;
        params.height = height;
        sparkle_.window_manager_.updateViewLayout(this, params);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        if (user == 0) {return;}
        //surface_changed(user, holder.getSurface());
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        if (user == 0) {return;}
        //surface_changed(user, holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        if (user == 0) {return;}
        //surface_changed(user, holder.getSurface());
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
                    //touch_down(user, 0, event.getX(), event.getY());
                    return true;
                case MotionEvent.ACTION_UP:
                    //touch_up(user, 0, event.getX(), event.getY());
                    return true;
                case MotionEvent.ACTION_MOVE:
                    //touch_motion(user, 0, event.getX(), event.getY());
                    return true;
            }
        }
        else if (hasSource(source, InputDevice.SOURCE_MOUSE))
        {
            switch (event.getAction())
            {
                case MotionEvent.ACTION_BUTTON_PRESS:
                    //mouse_button_press(user, event.getActionButton());
                    return true;
                case MotionEvent.ACTION_BUTTON_RELEASE:
                    //mouse_button_release(user, event.getActionButton());
                    return true;
                case MotionEvent.ACTION_HOVER_MOVE:
                    //mouse_pointer_motion(user, event.getX(), event.getY());
                    return true;
                case MotionEvent.ACTION_MOVE:
                    //mouse_pointer_motion(user, event.getX(), event.getY());
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
            //sparkle.hide_all(sparkle.user);
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
        {
        }

        //key_press(user, keyCode);
        return true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        if (user == 0) {return false;}

        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
        }
        else if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
        {
        }

        //key_release(user, keyCode);
        return true;
    }


    //public native void surface_changed(long user, Surface surface);

    //public native void touch_down(long user, int id, float x, float y);
    //public native void touch_up(long user, int id, float x, float y);
    //public native void touch_motion(long user, int id, float x, float y);

    //public native void key_press(long user, int code);
    //public native void key_release(long user, int code);

    //public native void mouse_button_press(long user, int button);
    //public native void mouse_button_release(long user, int button);
    //public native void mouse_pointer_motion(long user, float x, float y);
    //public native void mouse_pointer_enter(long user);
    //public native void mouse_pointer_leave(long user);

    SparkleService sparkle_;
    long user = 0;
    WindowManager.LayoutParams params;
}
