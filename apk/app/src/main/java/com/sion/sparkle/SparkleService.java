package com.sion.sparkle;

// Basic
import androidx.annotation.Keep;
import android.app.Service;
import android.os.IBinder;
import android.content.Intent;

// System services
import android.content.Context;

// Notification
import android.app.NotificationManager;
import android.app.Notification;

// Queue
import android.os.MessageQueue;
import android.os.Looper;

// FD listener
import android.os.ParcelFileDescriptor;
import java.io.FileDescriptor;

// Window manager
import android.view.WindowManager;

// Display size
import android.util.DisplayMetrics;


import android.util.Log;


public class SparkleService extends Service
{
    @Override
    public void onDestroy()
    {
        Log.i("Sparkle", "Stopping service...");

        notificationManager.cancel(0);

        native_destroy(native_);
    }

    @Override
    public void onCreate()
    {
        Log.i("Sparkle", "Starting service...");

        queue_ = Looper.myQueue();

        notificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
        window_manager_ = (WindowManager)getSystemService(Context.WINDOW_SERVICE);

        notification = new Notification.Builder(this)
            //.setContentTitle("Title")
            .setContentText("Sparkle")
            .setSmallIcon(R.drawable.notification_icon)
            .setOngoing(true)
            .build();

        notificationManager.notify(0, notification);

        native_ = native_create();
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Keep
    public void add_fd_listener(int fd, long listener)
    {
        ParcelFileDescriptor pfd = ParcelFileDescriptor.adoptFd(fd);
        FileDescriptor fd__ = pfd.getFileDescriptor();
        queue_.addOnFileDescriptorEventListener(fd__, MessageQueue.OnFileDescriptorEventListener.EVENT_INPUT, new MyOnFileDescriptorEventListener(listener));
        Log.i("Sparkle", "fd listener added");
    }

    @Keep
    public void remove_fd_listener(int fd)
    {
        ParcelFileDescriptor pfd = ParcelFileDescriptor.adoptFd(fd);
        FileDescriptor fd__ = pfd.getFileDescriptor();
        queue_.removeOnFileDescriptorEventListener(fd__);
        Log.i("Sparkle", "fd listener removed");
    }

    public int display_width()
    {
        DisplayMetrics display_metrics = new DisplayMetrics();
        window_manager_.getDefaultDisplay().getMetrics(display_metrics);
        return display_metrics.widthPixels;
    }

    public int display_height()
    {
        DisplayMetrics display_metrics = new DisplayMetrics();
        window_manager_.getDefaultDisplay().getMetrics(display_metrics);
        return display_metrics.heightPixels;
    }

    public native long native_create();
    public native void native_destroy(long native__);
    public native void fd_event(long listener);

    MessageQueue queue_;
    NotificationManager notificationManager;
    Notification notification;
    long native_;
    WindowManager window_manager_;


    static
    {
        System.loadLibrary("sparkle");
    }

    public class MyOnFileDescriptorEventListener implements MessageQueue.OnFileDescriptorEventListener
    {
        MyOnFileDescriptorEventListener(long listener)
        {
            listener_ = listener;
        }

        @Override
        public int onFileDescriptorEvents(FileDescriptor fd, int events)
        {
            fd_event(listener_);
            return EVENT_INPUT;
        }

        long listener_;
    }
}
