package com.sion.sparkle;

// Basic
import androidx.annotation.Keep;
import android.os.Build;
import android.app.Activity;
import android.os.Bundle;

// Start/Stop service
import android.content.Intent; // XXX3 Basic

// Request permissions
import android.provider.Settings;
import android.net.Uri;

// UI
import android.widget.Button;
import android.widget.LinearLayout;
import android.view.View; // Button callback

// Log
import android.util.Log;


public class MainActivity extends Activity
{
    public static final String EXTRA_FILE = "com.sion.sparkle.FILE";

    @Override
    protected void onDestroy()
    {
        native_destroy(native_);

        super.onDestroy();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && !Settings.canDrawOverlays(this))
        {
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION);
            intent.setData(Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, 1);
        }

        Button button1 = new Button(this);
        button1.setText("Start");
        button1.setOnClickListener(new Button.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(MainActivity.this, SparkleService.class);

                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                    startForegroundService(intent);
                else
                    startService(intent);

                native_start(native_);
            }
        });

        Button button2 = new Button(this);
        button2.setText("Stop");
        button2.setOnClickListener(new Button.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(MainActivity.this, SparkleService.class);
                stopService(intent);

                native_stop(native_);
            }
        });

        Button button3 = new Button(this);
        button3.setText("Edit settings.lua");
        button3.setOnClickListener(new Button.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(MainActivity.this, SparkleEditorActivity.class);
                String file = "settings.lua";
                intent.putExtra(EXTRA_FILE, file);
                startActivity(intent);
            }
        });

        Button button4 = new Button(this);
        button4.setText("Edit user.sh");
        button4.setOnClickListener(new Button.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(MainActivity.this, SparkleEditorActivity.class);
                String file = "user.sh";
                intent.putExtra(EXTRA_FILE, file);
                startActivity(intent);
            }
        });

        Button button5 = new Button(this);
        button5.setText("Check log");
        button5.setOnClickListener(new Button.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(MainActivity.this, SparkleEditorActivity.class);
                String file = "log.txt";
                intent.putExtra(EXTRA_FILE, file);
                startActivity(intent);
            }
        });

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.addView(button1);
        layout.addView(button2);
        layout.addView(button3);
        layout.addView(button4);
        layout.addView(button5);


        setContentView(layout);

        native_ = native_create();
        native_setup(native_);
    }

    @Keep
    public String files_dir()
    {
        return getFilesDir().getAbsolutePath();
    }

    @Keep
    public String home_dir()
    {
        return getApplicationInfo().dataDir;
    }

    static
    {
        System.loadLibrary("sparkle");
    }

    private native long native_create();
    private native void native_destroy(long native__);
    private native void native_setup(long native__);
    private native void native_start(long native__);
    private native void native_stop(long native__);

    private long native_;
}
