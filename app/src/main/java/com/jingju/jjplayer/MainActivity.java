package com.jingju.jjplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.media.AudioManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.jingju.jjplayer.R;

/**
 * todo
 *  1、播放的的view的相关的设置（url的设置）
 *
 *  2、 创建并初始化player
 *
 *  3、 播放相关视频
 *
 */

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private Remuxer remuxer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView decode = findViewById(R.id.decode);
        TextView remux = findViewById(R.id.remux);

        decode.setOnClickListener(this);
        remux.setOnClickListener(this);
        remuxer = new Remuxer();

//        getBestSampleRate();

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();


    public int getBestSampleRate()
    {
        if (Build.VERSION.SDK_INT >= 17) {
            AudioManager am = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
            String sampleRateString = am.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
            int sampleRate = sampleRateString == null ? 44100 : Integer.parseInt(sampleRateString);
            Log.d("samplerate","sampleRate=="+sampleRate);
            return sampleRate;
        } else {
            return 44100;
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.decode:
                Intent intent = new Intent();
                intent.setClass(MainActivity.this,VideoActivity.class);
                MainActivity.this.startActivity(intent);
                break;
            case R.id.remux:
                 remuxer.remux("");//暂时传空

                break;
        }
    }
}
