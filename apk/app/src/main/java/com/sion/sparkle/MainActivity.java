package com.sion.sparkle;

// Basic
import androidx.annotation.Keep;
import android.app.Activity;
import android.os.Bundle;

// Start/Stop service
import android.content.Intent;

// Request permissions
import android.provider.Settings;
import android.net.Uri;

// UI
import android.widget.Button;
import android.widget.LinearLayout;
import android.view.View; // Button callback

// Check version
import android.os.Build; // XXX3 Basic

// Assets
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;

import android.util.Log;


public class MainActivity extends Activity
{
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

        setup();

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

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.addView(button1);
        layout.addView(button2);


        setContentView(layout);

        native_ = native_create();
    }

    private void setup()
    {
        setPermissions(getApplicationInfo().dataDir, true, true, true, true, false, true);
        setPermissions(files_dir(), true, true, true, true, false, true);

        copyAsset("settings.lua", "settings.lua");
        setPermissions(files_dir() + "/settings.lua", true, true, false, true, true, false);

        copyAsset("sparkle.lua", "sparkle.lua");
        setPermissions(files_dir() + "/sparkle.lua", true, true, false, true, false, false);

        copyAsset("user.lua", "user.lua");
        setPermissions(files_dir() + "/user.lua", true, true, false, true, false, false);
    }

    private void copyAsset(String source, String destination)
    {
        File file = new File(files_dir(), destination);

        if (file.exists())
            return;

        try
        {
            InputStream in = getAssets().open(source);
            OutputStream out = new FileOutputStream(file);

            byte[] buffer = new byte[1024];

            int read = in.read(buffer);
            while (read != -1)
            {
                out.write(buffer, 0, read);
                read = in.read(buffer);
            }

            out.close();
            in.close();
        }
        catch (IOException e)
        {
            Log.e("Sparkle", e.getMessage());
        }
    }

    private void setPermissions(String path, boolean r, boolean w, boolean x, boolean r1, boolean w1, boolean x1)
    {
        File file = new File(path);
        file.setReadable(r, !r1);
        file.setWritable(w, !w1);
        file.setExecutable(x, !x1);
    }

    @Keep
    public String files_dir()
    {
        return getFilesDir().getAbsolutePath();
    }

    static
    {
        System.loadLibrary("sparkle");
    }

    private native long native_create();
    private native void native_destroy(long native__);
    private native void native_start(long native__);
    private native void native_stop(long native__);

    private long native_;
}
