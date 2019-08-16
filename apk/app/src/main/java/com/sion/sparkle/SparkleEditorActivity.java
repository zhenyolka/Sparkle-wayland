package com.sion.sparkle;

// Basic
import androidx.annotation.Keep;
import android.os.Build;
import android.app.Activity;
import android.os.Bundle;

// Get message
import android.content.Intent; // XXX3 Basic

// UI
import android.widget.Button;
import android.widget.LinearLayout;
import android.view.ViewGroup;
import android.view.View; // Button callback
import android.view.Gravity;
import android.text.method.ScrollingMovementMethod;
import android.widget.TextView;
import android.widget.EditText;
import android.graphics.Typeface;
import android.text.InputType;
import android.graphics.Color;

// File R/W
import java.io.File;
import java.io.IOException;
import java.io.FileInputStream;
import java.io.FileOutputStream;

// Log
import android.util.Log;


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
        boolean read_only = intent.getBooleanExtra(MainActivity.EXTRA_FILE_RO, true);


        Button button1 = new Button(this);
        button1.setText("Save");
        button1.setOnClickListener(new Button.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                try
                {
                    write_file(file_, editText_.getText().toString());
                }
                catch (IOException e)
                {
                    Log.e("Sparkle", e.getMessage());
                }
            }
        });

        if (read_only)
        {
            editText_ = new TextView(this);
            editText_.setMovementMethod(new ScrollingMovementMethod());
            editText_.setTextIsSelectable(true);
        }
        else
        {
            editText_ = new EditText(this);
            editText_.setGravity(Gravity.LEFT | Gravity.TOP);
            editText_.setHorizontallyScrolling(true);
            editText_.setInputType(editText_.getInputType() | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
        }

        editText_.setTypeface(Typeface.MONOSPACE, Typeface.NORMAL);
        editText_.setTextSize(10.0f);
        editText_.setBackgroundColor(Color.BLACK);
        editText_.setTextColor(Color.WHITE);

        try
        {
            editText_.setText(read_file(file_));
        }
        catch (IOException e)
        {
            Log.e("Sparkle", e.getMessage());
        }


        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);

        LinearLayout.LayoutParams params1 = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, 0, 1);
        LinearLayout.LayoutParams params2 = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);

        layout.addView(editText_, params1);
        if (!read_only)
            layout.addView(button1, params2);


        layout.setBackgroundColor(Color.BLACK);
        setContentView(layout);
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

    private TextView editText_;
    private String file_;
}
