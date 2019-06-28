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
import android.app.PendingIntent; // Actions

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

// Broadcast receiver
import android.content.BroadcastReceiver;
import android.content.IntentFilter;

import android.util.Log;


// TMP
import java.util.Timer;
import java.util.TimerTask;


public class SparkleService extends Service
{
    public static final String ACTION_HIDE = "com.sion.sparkle.ACTION_HIDE";
    public static final String ACTION_SHOW = "com.sion.sparkle.ACTION_SHOW";
    public static final String ACTION_STOP = "com.sion.sparkle.ACTION_STOP";

    @Override
    public void onDestroy()
    {
        Log.i("Sparkle", "Stopping service...");

        unregisterReceiver(receiver_);

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
            .build();

        notificationManager.notify(0, notification);

        receiver_ = new BroadcastReceiver()
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

        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_HIDE);
        filter.addAction(ACTION_SHOW);
        filter.addAction(ACTION_STOP);
        registerReceiver(receiver_, filter);

        native_ = native_create();


        Timer myTimer = new Timer();
        myTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                Log.i("Sparkle", "TMR");
            }

        }, 0, 1000);


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
    BroadcastReceiver receiver_;


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
