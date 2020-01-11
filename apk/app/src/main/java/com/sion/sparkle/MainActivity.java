package com.sion.sparkle;

// Basic
import androidx.annotation.Keep;
import android.os.Build;
import android.content.Intent;
import android.content.Context;
import android.util.Log;

import android.app.Activity;
import android.os.Bundle;

// Request permissions
import android.provider.Settings;
import android.net.Uri;

// UI
import android.widget.Button;
import android.view.View; // Button callback


public class MainActivity extends Activity
{
    public static final String EXTRA_FILE = "com.sion.sparkle.FILE";
    public static final String EXTRA_FILE_LOG = "com.sion.sparkle.FILE_LOG";
    public static final String EXTRA_QUIET = "com.sion.sparkle.QUIET";

    @Override
    protected void onDestroy()
    {
        Log.i("Sparkle", "Stopping SparkleMainActivity");

        native_destroy(native_);

        super.onDestroy();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        Log.i("Sparkle", "Starting SparkleMainActivity");

        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        WereApplication.getInstance(getApplicationContext());

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && !Settings.canDrawOverlays(this))
        {
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION);
            intent.setData(Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, 1);
        }

        Intent intent__ = getIntent();
        // am start -n com.sion.sparkle/.MainActivity --ez com.sion.sparkle.QUIET 1
        boolean quiet = intent__.getBooleanExtra(MainActivity.EXTRA_QUIET, false);

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
                intent.putExtra(EXTRA_FILE_LOG, false);
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
                String file = "user.lua";
                intent.putExtra(EXTRA_FILE, file);
                intent.putExtra(EXTRA_FILE_LOG, false);
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
                intent.putExtra(EXTRA_FILE_LOG, true);
                startActivity(intent);
            }
        });

        native_ = native_create();
        native_setup(native_);

        if (quiet)
        {
            Intent intent = new Intent(MainActivity.this, SparkleService.class);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                startForegroundService(intent);
            else
                startService(intent);
            finish();
        }
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
