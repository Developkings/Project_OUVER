package org.ouver.demo;

import android.media.MediaRecorder;

import java.io.IOException;

public class SoundMeter {
    private MediaRecorder mRecorder = null;

    public void start() throws IOException {
        if (mRecorder == null) {
            mRecorder = new MediaRecorder();
            mRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
            mRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
            mRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
            mRecorder.setOutputFile("/dev/null");
            mRecorder.prepare();
            mRecorder.start();
        }
    }

    public void stop() {
        if (mRecorder != null) {
            mRecorder.stop();
            mRecorder.release();
            mRecorder = null;
        }
    }

    public float getAmplitude() {

        float dB = 0;
        float volume = mRecorder.getMaxAmplitude();
        if (mRecorder != null) {
            if (volume > 0 && volume < 1000000) {
                dB = 20 * (float) (Math.log10(volume));
            }

            return dB;
        } else {
            return 0;

        }
    }
}
