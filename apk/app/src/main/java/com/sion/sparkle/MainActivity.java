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
                startService(intent);
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
