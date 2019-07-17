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


public class MainActivity extends Activity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        if (!Settings.canDrawOverlays(this))
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
                // Crashy
                //builder.redirectOutput(new File("/data/data/com.sion.sparkle/user_log.txt"));
                //builder.redirectError(new File("/data/data/com.sion.sparkle/user_log.txt"));

                try
                {
                    Process process = builder.start();
                    Runtime.getRuntime().exec("chmod 666 /data/data/com.sion.sparkle/user_log.txt"); // XXX3
                }
                catch (IOException e)
                {
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
}
