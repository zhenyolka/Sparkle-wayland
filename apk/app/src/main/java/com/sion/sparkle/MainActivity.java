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
import android.widget.ImageView;
import android.graphics.Color;

// Log
import android.util.Log;


public class MainActivity extends Activity
{
    public static final String EXTRA_FILE = "com.sion.sparkle.FILE";
    public static final String EXTRA_FILE_RO = "com.sion.sparkle.FILE_RO";

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
        setContentView(R.layout.main);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && !Settings.canDrawOverlays(this))
        {
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION);
            intent.setData(Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, 1);
        }

        Button button1 = (Button)findViewById(R.id.button1);
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

        Button button2 = (Button)findViewById(R.id.button2);
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

        Button button3 = (Button)findViewById(R.id.button3);
        button3.setOnClickListener(new Button.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(MainActivity.this, SparkleEditorActivity.class);
                String file = "settings.lua";
                intent.putExtra(EXTRA_FILE, file);
                intent.putExtra(EXTRA_FILE_RO, false);
                startActivity(intent);
            }
        });

        Button button4 = (Button)findViewById(R.id.button4);
        button4.setOnClickListener(new Button.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(MainActivity.this, SparkleEditorActivity.class);
                String file = "user.sh";
                intent.putExtra(EXTRA_FILE, file);
                intent.putExtra(EXTRA_FILE_RO, false);
                startActivity(intent);
            }
        });

        Button button5 = (Button)findViewById(R.id.button5);
        button5.setOnClickListener(new Button.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Intent intent = new Intent(MainActivity.this, SparkleEditorActivity.class);
                String file = "log.txt";
                intent.putExtra(EXTRA_FILE, file);
                intent.putExtra(EXTRA_FILE_RO, true);
                startActivity(intent);
            }
        });

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
