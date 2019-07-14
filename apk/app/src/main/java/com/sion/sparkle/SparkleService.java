package com.sion.sparkle;

// Basic
import androidx.annotation.Keep;
import android.app.Service;
import android.os.IBinder;
import android.content.Intent;

// System services
import android.content.Context; // Basic

// Notification
import androidx.core.app.NotificationCompat;
import android.app.NotificationManager;
import android.app.Notification;
import android.app.NotificationChannel;
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

// Map
import java.util.Map;
import java.util.HashMap;

// Check version
import android.os.Build;


import android.util.Log;


// Temporary
//import java.util.Timer;
//import java.util.TimerTask;


public class SparkleService extends Service
{
    public static final String ACTION_HIDE = "com.sion.sparkle.ACTION_HIDE";
    public static final String ACTION_SHOW = "com.sion.sparkle.ACTION_SHOW";
    public static final String ACTION_STOP = "com.sion.sparkle.ACTION_STOP";
    public static final String CHANNEL_ID = "SparkleChannel";

    @Override
    public void onDestroy()
    {
        Log.i("Sparkle", "Stopping service...");

        unregisterReceiver(receiver_);

        native_destroy(native_);
    }

    @Override
    public void onCreate()
    {
        Log.i("Sparkle", "Starting service...");

        queue_ = Looper.myQueue();

        //notificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
        //notificationManager.notify(1, notification);
        //notificationManager.cancel(1);

        window_manager_ = (WindowManager)getSystemService(Context.WINDOW_SERVICE);




        receiver_ = new BroadcastReceiver()
        {
            @Override
            public void onReceive(Context context, Intent intent)
            {
                String action = intent.getAction();

                if (action.equals(ACTION_STOP))
                {
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


        /*
        Timer myTimer = new Timer();
        myTimer.schedule(new TimerTask()
        {
            @Override
            public void run()
            {
                Log.i("Sparkle", "Timer");
            }

        }, 0, 1000);
        */

    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        createNotificationChannel();

        Intent intent1 = new Intent();
        intent1.setAction(ACTION_HIDE);
        PendingIntent pendingIntent1 = PendingIntent.getBroadcast(this, 0, intent1, 0);

        Intent intent2 = new Intent();
        intent2.setAction(ACTION_SHOW);
        PendingIntent pendingIntent2 = PendingIntent.getBroadcast(this, 0, intent2, 0);

        Intent intent3 = new Intent();
        intent3.setAction(ACTION_STOP);
        PendingIntent pendingIntent3 = PendingIntent.getBroadcast(this, 0, intent3, 0);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
            //.setContentTitle("Title")
            .setContentText("Sparkle")
            .setSmallIcon(R.drawable.notification_icon)
            .setPriority(NotificationCompat.PRIORITY_LOW)
            //.setOngoing(true)
            .addAction(R.drawable.notification_icon, "Hide", pendingIntent1)
            .addAction(R.drawable.notification_icon, "Show", pendingIntent2)
            .addAction(R.drawable.notification_icon, "Stop", pendingIntent3);

        Notification notification = builder.build();

        startForeground(1, notification);

        return START_NOT_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    private void createNotificationChannel()
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            CharSequence name = CHANNEL_ID;
            String description = CHANNEL_ID;
            int importance = NotificationManager.IMPORTANCE_LOW;
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, name, importance);
            channel.setDescription(description);

            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            notificationManager.createNotificationChannel(channel);
        }
    }

    @Keep
    public void add_fd_listener(int fd, long user)
    {
        ParcelFileDescriptor pfd = ParcelFileDescriptor.adoptFd(fd);
        FileDescriptor fd__ = pfd.getFileDescriptor();
        queue_.addOnFileDescriptorEventListener(fd__, MessageQueue.OnFileDescriptorEventListener.EVENT_INPUT, new MyOnFileDescriptorEventListener(user));
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

    @Keep
    public void add_idle_handler(long user)
    {
        MyIdleHandler handler = new MyIdleHandler(user);
        queue_.addIdleHandler(handler);
        idle_handlers_.put(user, handler);

        Log.i("Sparkle", "idle handler added");
    }

    @Keep
    public void remove_idle_handler(long user)
    {
        MyIdleHandler handler = idle_handlers_.get(user);
        queue_.removeIdleHandler(handler);
        idle_handlers_.remove(user);

        Log.i("Sparkle", "idle handler removed");
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

        int status_bar_height = 0;
        int resource_id = getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resource_id > 0)
            status_bar_height = getResources().getDimensionPixelSize(resource_id);

        Log.i("Sparkle", String.format("status_bar_height = %d", status_bar_height));

        return display_metrics.heightPixels - status_bar_height;
    }

    public native long native_create();
    public native void native_destroy(long native__);
    public native void fd_event(long user);
    public native void idle_event(long user);

    MessageQueue queue_;
    //NotificationManager notificationManager;
    //Notification notification;
    long native_;
    WindowManager window_manager_;
    BroadcastReceiver receiver_;
    //Map<long, MyOnFileDescriptorEventListener> on_file_descriptor_event_listeners_;
    Map<Long, MyIdleHandler> idle_handlers_ = new HashMap<Long, MyIdleHandler>();


    static
    {
        System.loadLibrary("sparkle");
    }

    public class MyOnFileDescriptorEventListener implements MessageQueue.OnFileDescriptorEventListener
    {
        MyOnFileDescriptorEventListener(long user)
        {
            user_ = user;
        }

        @Override
        public int onFileDescriptorEvents(FileDescriptor fd, int events)
        {
            fd_event(user_);
            return EVENT_INPUT;
        }

        long user_;
    }

    public class MyIdleHandler implements MessageQueue.IdleHandler
    {
        MyIdleHandler(long user)
        {
            user_ = user;
        }

        @Override
        public boolean queueIdle()
        {
            idle_event(user_);
            return true;
        }

        long user_;
    }
}
