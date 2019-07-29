package com.sion.sparkle;

// Basic
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

// Script
import java.io.File;
import java.io.IOException;

// Check version
import android.os.Build;

// Assets
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;

import android.util.Log;


public class MainActivity extends Activity
{
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

                ProcessBuilder builder = new ProcessBuilder("/system/bin/sh", "-c",
                    "/data/data/com.sion.sparkle/user.sh start > /data/data/com.sion.sparkle/user_log.txt 2>&1");

                try
                {
                    Process process = builder.start();
                    Runtime.getRuntime().exec("chmod 666 /data/data/com.sion.sparkle/user_log.txt"); // XXX3
                }
                catch (IOException e)
                {
                    Log.e("Sparkle", e.getMessage());
                }
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
            }
        });

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.addView(button1);
        layout.addView(button2);


        setContentView(layout);
    }

    private void setup()
    {
        setPermissions(getApplicationInfo().dataDir, true, true, true, true, false, true);
        copyAsset("settings.lua");
        setPermissions(getApplicationInfo().dataDir + "/" + "settings.lua", true, true, false, true, true, false);
    }

    private void copyAsset(String path)
    {
        File file = new File(getApplicationInfo().dataDir, path);

        if (file.exists())
            return;

        try
        {
            InputStream in = getAssets().open(path);
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
}
