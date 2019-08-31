package com.sion.sparkle;

// Basic
import androidx.annotation.Keep;
import android.os.Build;
import android.content.Intent;
import android.content.Context;
import android.util.Log;

import android.app.Activity;
import android.os.Bundle;

// UI
import android.widget.ScrollView;
import android.widget.Button;
import android.view.View; // Button callback
import android.widget.TextView;
import android.widget.EditText;

// File R/W
import java.io.File;
import java.io.IOException;
import java.io.FileInputStream;
import java.io.FileOutputStream;


public class SparkleEditorActivity extends Activity
{
    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();
        file_ = intent.getStringExtra(MainActivity.EXTRA_FILE);
        boolean read_only = intent.getBooleanExtra(MainActivity.EXTRA_FILE_LOG, true);

        if (read_only)
        {
            setContentView(R.layout.viewer);

            TextView text = (TextView)findViewById(R.id.text);
            final ScrollView scroller = (ScrollView)findViewById(R.id.scroller);

            text.setText("");

            try
            {
                text.append(read_file(file_ + ".old"));

            }
            catch (IOException e)
            {
                Log.e("Sparkle", e.getMessage());
            }

            try
            {
                text.append(read_file(file_));
            }
            catch (IOException e)
            {
                Log.e("Sparkle", e.getMessage());
            }

            scroller.post(new Runnable()
            {
                @Override
                public void run()
                {
                    scroller.fullScroll(View.FOCUS_DOWN);
                }
            });

        }
        else
        {
            setContentView(R.layout.editor);

            EditText text = (EditText)findViewById(R.id.text);
            text.setHorizontallyScrolling(true);

            try
            {
                text.setText(read_file(file_));
            }
            catch (IOException e)
            {
                Log.e("Sparkle", e.getMessage());
            }

            Button button_save = (Button)findViewById(R.id.button_save);
            button_save.setOnClickListener(new Button.OnClickListener()
            {
                @Override
                public void onClick(View v)
                {
                    try
                    {
                        EditText text = (EditText)findViewById(R.id.text);
                        write_file(file_, text.getText().toString());
                    }
                    catch (IOException e)
                    {
                        Log.e("Sparkle", e.getMessage());
                    }
                }
            });
        }

    }

    private String read_file(String name) throws IOException
    {
        File file = new File(getFilesDir(), name);
        int length = (int)file.length();
        byte[] buffer = new byte[length];
        FileInputStream stream = new FileInputStream(file);
        stream.read(buffer);
        stream.close();

        return new String(buffer);
    }

    private void write_file(String name, String text) throws IOException
    {
        File file = new File(getFilesDir(), name);
        FileOutputStream stream = new FileOutputStream(file);
        stream.write(text.getBytes());
        stream.close();
    }

    private String file_;
}
