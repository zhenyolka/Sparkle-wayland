package com.sion.sparkle;

// Basic
import androidx.annotation.Keep;
import android.util.Log;

// Queue
import android.os.MessageQueue;
import android.os.Looper;

// FD listener
import android.os.ParcelFileDescriptor;
import java.io.FileDescriptor;


public class WereApplication
{
    private static WereApplication instance_ = null;
    public static synchronized WereApplication getInstance()
    {
        if (instance_ == null)
            instance_ = new WereApplication();
        return instance_;
    }

    WereApplication()
    {
        Log.i("Sparkle", "WereApplication");

        queue_ = Looper.myQueue();
        fd_listener_ = new MyOnFileDescriptorEventListener();
        idle_handler_ = new MyIdleHandler();
    }

    void reference()
    {
        if (reference_count_ == 0)
        {
            native_ = native_create();
            Log.i("Sparkle", "WereApplication native created");
        }
        reference_count_ += 1;
    }

    void unreference()
    {
        reference_count_ -= 1;
        if (reference_count_ == 0)
        {
            native_destroy(native_);
            Log.i("Sparkle", "WereApplication native destroyed");
        }
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

    public void set_files_dir(String path)
    {
        files_dir_ = path;
    }

    public void set_home_dir(String path)
    {
        home_dir_ = path;
    }

    @Keep
    public String files_dir()
    {
        return files_dir_;
    }

    @Keep
    public String home_dir()
    {
        return home_dir_;
    }

    private native long native_create();
    private native void native_destroy(long native__);
    public native void native_loop_fd_event();
    public native void native_loop_idle_event();

    private int reference_count_ = 0;
    long native_ = 0;

    MessageQueue queue_;
    MyOnFileDescriptorEventListener fd_listener_ = null;
    FileDescriptor fd_listener_fd_ = null;
    MyIdleHandler idle_handler_ = null;
    String files_dir_;
    String home_dir_;

    public class MyOnFileDescriptorEventListener implements MessageQueue.OnFileDescriptorEventListener
    {
        @Override
        public int onFileDescriptorEvents(FileDescriptor fd, int events)
        {
            native_loop_fd_event();
            return EVENT_INPUT;
        }
    }

    public class MyIdleHandler implements MessageQueue.IdleHandler
    {
        @Override
        public boolean queueIdle()
        {
            native_loop_idle_event();
            return true;
        }
    }
}
