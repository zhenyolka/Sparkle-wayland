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
import android.widget.RemoteViews; // Custom notification

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
//import java.util.Map;
//import java.util.HashMap;
//map.put(k, v);
//map.remove(k);
//Map<Long, MyIdleHandler> idle_handlers_ = new HashMap<Long, MyIdleHandler>();

// Check version
import android.os.Build;


import android.util.Log;


// Timer
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
        //filter.addAction(ACTION_HIDE);
        //filter.addAction(ACTION_SHOW);
        filter.addAction(ACTION_STOP);
        registerReceiver(receiver_, filter);


        fd_listener_ = new MyOnFileDescriptorEventListener(0);
        idle_handler_ = new MyIdleHandler(0);


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

        RemoteViews notificationLayout = new RemoteViews(getPackageName(), R.layout.notification_small);

        Intent intent1 = new Intent();
        intent1.setAction(ACTION_HIDE);
        PendingIntent pendingIntent1 = PendingIntent.getBroadcast(this, 0, intent1, 0);

        Intent intent2 = new Intent();
        intent2.setAction(ACTION_SHOW);
        PendingIntent pendingIntent2 = PendingIntent.getBroadcast(this, 0, intent2, 0);

        Intent intent3 = new Intent();
        intent3.setAction(ACTION_STOP);
        PendingIntent pendingIntent3 = PendingIntent.getBroadcast(this, 0, intent3, 0);

        notificationLayout.setOnClickPendingIntent(R.id.button1, pendingIntent1);
        notificationLayout.setOnClickPendingIntent(R.id.button2, pendingIntent2);
        notificationLayout.setOnClickPendingIntent(R.id.button3, pendingIntent3);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
            //.setContentTitle("Title")
            //.setContentText("Sparkle")
            .setSmallIcon(R.drawable.notification_icon)
            .setPriority(NotificationCompat.PRIORITY_LOW)
            //.setOngoing(true)
            //.addAction(R.drawable.notification_icon, "Hide", pendingIntent1)
            //.addAction(R.drawable.notification_icon, "Show", pendingIntent2)
            //.addAction(R.drawable.notification_icon, "Stop", pendingIntent3)
            .setStyle(new NotificationCompat.DecoratedCustomViewStyle())
            .setCustomContentView(notificationLayout);



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
    public String files_dir()
    {
        return getFilesDir().getAbsolutePath();
    }

    @Keep
    public void enable_native_loop(int fd)
    {
        ParcelFileDescriptor parcel_fd = ParcelFileDescriptor.adoptFd(fd);
        fd_listener_fd_ = parcel_fd.getFileDescriptor();
        queue_.addOnFileDescriptorEventListener(fd_listener_fd_, MessageQueue.OnFileDescriptorEventListener.EVENT_INPUT, fd_listener_);
        queue_.addIdleHandler(idle_handler_);
    }

    @Keep
    public void disable_native_loop()
    {
        queue_.removeIdleHandler(idle_handler_);
        queue_.removeOnFileDescriptorEventListener(fd_listener_fd_);
        fd_listener_fd_ = null;
    }

    @Keep
    public int display_width()
    {
        DisplayMetrics display_metrics = new DisplayMetrics();
        window_manager_.getDefaultDisplay().getMetrics(display_metrics);
        return display_metrics.widthPixels;
    }

    @Keep
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

    private native long native_create();
    private native void native_destroy(long native__);
    public native void native_loop_fd_event();
    public native void native_loop_idle_event();

    MessageQueue queue_;
    //NotificationManager notificationManager;
    long native_;
    WindowManager window_manager_;
    BroadcastReceiver receiver_;

    MyOnFileDescriptorEventListener fd_listener_ = null;
    FileDescriptor fd_listener_fd_ = null;
    MyIdleHandler idle_handler_ = null;

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
            native_loop_fd_event();
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
            native_loop_idle_event();
            return true;
        }

        long user_;
    }
}
