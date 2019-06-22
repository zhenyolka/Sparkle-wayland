package com.sion.sparkle;

//import androidx.appcompat.app.AppCompatActivity;
import android.app.Activity;
import android.os.Bundle;
import android.content.Intent;

import android.provider.Settings;
import android.net.Uri;

import android.util.Log;

public class MainActivity extends Activity // AppCompatActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        if (!Settings.canDrawOverlays(this))
        {
            Intent myIntent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION);
            myIntent.setData(Uri.parse("package:" + getPackageName()));
            startActivityForResult(myIntent, 1);
        }
        else
        {
            Intent intent = new Intent(this, SparkleService.class);
            startService(intent);
            finish();
        }
    }
}
