package com.sion.sparkle;

import androidx.annotation.Keep;
import android.app.Service;
import android.os.IBinder;
import android.content.Intent;

import android.os.Handler;
import android.os.MessageQueue;
import android.os.Looper;
import java.io.FileDescriptor;
import android.os.ParcelFileDescriptor;

import android.content.Context;
import android.view.WindowManager;
import android.view.Display;
import android.util.DisplayMetrics;

import android.app.NotificationManager;
import android.app.Notification;
import android.app.PendingIntent;

import android.content.BroadcastReceiver;
import android.content.IntentFilter;


import android.util.Log;


public class SparkleService extends Service
{
    Handler handler;
    MessageQueue queue;
    long user;
    ParcelFileDescriptor pfd;
    FileDescriptor fd;
    MyOnFileDescriptorEventListener fdListener;

    WindowManager windowManager;
    NotificationManager notificationManager;

    public static final String ACTION_HIDE = "com.sion.sparkle.ACTION_HIDE";
    public static final String ACTION_SHOW = "com.sion.sparkle.ACTION_SHOW";
    public static final String ACTION_STOP = "com.sion.sparkle.ACTION_STOP";

    Notification notification;


    private final BroadcastReceiver receiver = new BroadcastReceiver()
    {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            String action = intent.getAction();

            if (action.equals(ACTION_HIDE))
            {
                Log.i("Sparkle", "Hide all");
                //hide_all(user);
            }
            else if (action.equals(ACTION_SHOW))
            {
                Log.i("Sparkle", "Show all");
                //show_all(user);
            }
            else if (action.equals(ACTION_STOP))
            {
                Log.i("Sparkle", "Stop");
                stopSelf();
            }
        }
    };

    @Override
    public void onDestroy()
    {
        notificationManager.cancel(0);
        unregisterReceiver(receiver);

        //native_destroy(user);
    }

    @Override
    public void onCreate()
    {
        handler = new Handler();

        queue = Looper.myQueue();

        fdListener = new MyOnFileDescriptorEventListener();

        windowManager = (WindowManager)getSystemService(Context.WINDOW_SERVICE);
        notificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);

        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_HIDE);
        filter.addAction(ACTION_SHOW);
        filter.addAction(ACTION_STOP);
        registerReceiver(receiver, filter);

        Intent intent1 = new Intent();
        intent1.setAction(ACTION_HIDE);
        PendingIntent pendingIntent1 = PendingIntent.getBroadcast(this, 0, intent1, 0);

        Intent intent2 = new Intent();
        intent2.setAction(ACTION_SHOW);
        PendingIntent pendingIntent2 = PendingIntent.getBroadcast(this, 0, intent2, 0);

        Intent intent3 = new Intent();
        intent3.setAction(ACTION_STOP);
        PendingIntent pendingIntent3 = PendingIntent.getBroadcast(this, 0, intent3, 0);

        notification = new Notification.Builder(this)
            //.setContentTitle("Title")
            .setContentText("Sparkle")
            .setSmallIcon(R.drawable.notification_icon)
            .setOngoing(true)
            .addAction(R.drawable.notification_icon, "Hide", pendingIntent1)
            .addAction(R.drawable.notification_icon, "Show", pendingIntent2)
            .addAction(R.drawable.notification_icon, "Stop", pendingIntent3)
            .setContentIntent(pendingIntent2)
            .build();

        notificationManager.notify(0, notification);

        //user = native_create();

/*
        handler.postDelayed(new Runnable()
        {
            @Override
            public void run()
            {
                native_event(user);
                handler.postDelayed(this, 1000 / 60);
            }
        }, 1000);
*/

        DisplayMetrics displayMetrics = new DisplayMetrics();
        windowManager.getDefaultDisplay().getMetrics(displayMetrics);
        int height = displayMetrics.heightPixels;
        int width = displayMetrics.widthPixels;

        //display_size(user, width, height);

    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    /*
    @Keep
    private SparkleView createView()
    {
        SparkleView view = new SparkleView(this);
        windowManager.addView(view, view.params);
        Log.i("Sparkle", "View added");

        return view;
    }

    @Keep
    private void destroyView(SparkleView view)
    {
        windowManager.removeView(view);
        Log.i("Sparkle", "View removed");
    }
    */

    @Keep
    public void setNativeFd(int fd)
    {
        this.pfd = ParcelFileDescriptor.adoptFd(fd);
        this.fd = pfd.getFileDescriptor();
        queue.addOnFileDescriptorEventListener(this.fd, MessageQueue.OnFileDescriptorEventListener.EVENT_INPUT, fdListener);
    }

    @Keep
    public void unsetNativeFd()
    {
        queue.removeOnFileDescriptorEventListener(this.fd);
    }

    static
    {
        System.loadLibrary("sparkle");
    }

    //public native long native_create();
    //public native void native_destroy(long user);
    //public native void native_event(long user);
    //public native void display_size(long user, int width, int height);
    //public native void show_all(long user);
    //public native void hide_all(long user);
    //public native void stop_server(long user);

    public class MyOnFileDescriptorEventListener implements MessageQueue.OnFileDescriptorEventListener
    {
        public int onFileDescriptorEvents(FileDescriptor fd, int events)
        {
            //native_event(user);
            return EVENT_INPUT;
        }
    }
}
