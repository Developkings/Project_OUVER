// Copyright 2019 Alpha Cephei Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package org.ouver.demo;

import android.Manifest;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.provider.Settings;
import android.speech.tts.TextToSpeech;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;
import androidx.preference.PreferenceManager;

import com.physicaloid.lib.Physicaloid;

import org.json.JSONException;
import org.json.JSONObject;
import org.vosk.LibVosk;
import org.vosk.LogLevel;
import org.vosk.Model;
import org.vosk.Recognizer;
import org.vosk.android.RecognitionListener;
import org.vosk.android.SpeechService;
import org.vosk.android.SpeechStreamService;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.Locale;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

import app.akexorcist.bluetotohspp.library.BluetoothSPP;
import app.akexorcist.bluetotohspp.library.BluetoothState;
import app.akexorcist.bluetotohspp.library.DeviceList;


public class OuverActivity extends Activity implements
        RecognitionListener {

    static private final int STATE_START = 0;
    static private final int STATE_READY = 1;
    static private final int STATE_DONE = 2;
    static private final int STATE_FILE = 3;
    static private final int STATE_MIC = 4;
    static private final int STATE_GET_NAME = 5;
    static private final int STATE_NAME_MODE = 6;


    /* Used to handle permission request */
    private static final int PERMISSIONS_REQUEST_RECORD_AUDIO = 1;
    private static final int PERMISSIONS_REQUEST_ALL_FILES_ACCESS = 2;


    private Model model;
    private SpeechService speechService;
    private SpeechStreamService speechStreamService;
    private TextView resultView;
    private String Connectedto;
    private boolean isconnectusb = false;
    private boolean isconnectbt = false;
    private boolean istosend = false;
    private boolean notificationdstatus = false;
    private boolean hasuser = false;
    private boolean mynameMODE = false;
    private boolean soundsensorMODE = false;
    private boolean soundsensorSTATUS = false;
    private String txtPreRecieved;
    private String appName;
    private String userName;


    public static final String mypreference = "mypref";
    public static final String Name = "nameKey";



    int counterfinal = 0;
    int soundsensorcount = 0;

    Physicaloid mPhysicaloid;
    BluetoothSPP bt;
    SharedPreferences sharedpreferences;
    TextToSpeech t1;
    SoundMeter soundMeter;

    @Override
    public void onCreate(Bundle state) {
        super.onCreate(state);
        setContentView(R.layout.main);
        getActionBar().hide();

        /* start notifications*/
        LocalBroadcastManager.getInstance(this).registerReceiver(onNotice, new IntentFilter("Msg"));

        /* start usb object*/
        mPhysicaloid = new Physicaloid(this);

        /* start bluetooth object*/
        bt = new BluetoothSPP(this);

        // Setup layout
        resultView = findViewById(R.id.result_text);
        setUiState(STATE_START);

        /*Vosk buttons*/
        findViewById(R.id.recognize_mic).setOnClickListener(view -> recognizeMicrophone());
        ((ToggleButton) findViewById(R.id.pause)).setOnCheckedChangeListener((view, isChecked) -> pause(isChecked));

        /*Views buttons*/
        findViewById(R.id.settingsLL).setVisibility(View.GONE);
        findViewById(R.id.settingsBT).setOnClickListener(view -> settingsAC());
        findViewById(R.id.callLL).setVisibility(View.GONE);
        findViewById(R.id.callBT).setOnClickListener(view -> callAC());
        LibVosk.setLogLevel(LogLevel.INFO);

        /*Usb buttons*/
        findViewById(R.id.usbconnectBT).setOnClickListener(view -> startUsb());

        /*Call Me buttons*/
        findViewById(R.id.callmeBT).setOnClickListener(view -> callmeAC());

        /*BT buttons*/
        findViewById(R.id.bluetoothconnectBT).setOnClickListener(view -> startBT());

        /*Notifications buttons*/
        findViewById(R.id.notificationsBT).setOnClickListener(view -> notificationsclicked());

        /*Languages buttons*/
        findViewById(R.id.languagesBTN).setOnClickListener(view -> languagesAC());

        /*Warning buttons*/
        findViewById(R.id.warningBTN).setOnClickListener(view -> playwarningAC());

        /*Soundsensor buttons*/
        findViewById(R.id.soundsensorBT).setOnClickListener(view -> soundsensorAC());

        /*Name buttons and input*/
        findViewById(R.id.getnameBT).setOnClickListener(view -> getnameclicked());
        findViewById(R.id.changeenameagainBT).setOnClickListener(view -> changenameagainclicked());
        sharedpreferences = getSharedPreferences(mypreference,
                Context.MODE_PRIVATE);
        if (sharedpreferences.contains(Name)) {
            userName = sharedpreferences.getString(Name, "there");
            hasuser = true;
            setUiState(STATE_GET_NAME);

        }

        LibVosk.setLogLevel(LogLevel.INFO);

           /* Check if user has given permission to record audio, init the model after permission is granted */
        int permissionCheck = ContextCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.RECORD_AUDIO);
        if (permissionCheck != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, PERMISSIONS_REQUEST_RECORD_AUDIO);
        } else {
            requestAllFilesAccessPermission();
        }
        soundMeter = new SoundMeter();

        t1=new TextToSpeech(getApplicationContext(), status -> {
            if(status != TextToSpeech.ERROR) {
                t1.setLanguage(Locale.UK);
            }
        });
    }

    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.main, menu);
        return true;
    }

    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_settings:
                startActivity(new Intent(this, SettingsActivity.class));
                return true;

            default:
                // The user's action was not recognized.
                // Invoke the superclass to handle it.
                return super.onOptionsItemSelected(item);
        }
    }

    private void requestAllFilesAccessPermission() {
        // Check if user has given all files access permission to record audio, init model after permission is granted
        if (Build.VERSION.SDK_INT >= 30) {
            Log.i(OuverActivity.class.getName(), "API level >= 30");
            if (!Environment.isExternalStorageManager()) {
                // Request permission
                try {
                    Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                    Uri uri = Uri.fromParts("package", getPackageName(), null);
                    intent.setData(uri);
                    startActivityForResult(intent, PERMISSIONS_REQUEST_ALL_FILES_ACCESS);
                } catch (android.content.ActivityNotFoundException e) {
                    setErrorState("Failed to request all files access permission");
                }
            } else {
                customInitModel();
            }
        } else {
            Log.i(OuverActivity.class.getName(), "API level < 30");
            // Request permission
            int permissionCheck = ContextCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.WRITE_EXTERNAL_STORAGE);
            if (permissionCheck != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, PERMISSIONS_REQUEST_ALL_FILES_ACCESS);
            } else {
                customInitModel();
            }
        }
    }

    private void downloadModel(String lang) {
        Log.i(OuverActivity.class.getName(), "Download model");

        String serverFilePath = "https://alphacephei.com/vosk/models/" + lang + ".zip";
        String fileName = lang + ".zip";

        String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/models";
        File tmpLocal = new File(path);
        if(!tmpLocal.exists()) {
            tmpLocal.mkdirs();
        }

        File file = new File(path, fileName);
        String localPath = file.getAbsolutePath();

        FileDownloadService.DownloadRequest downloadRequest = new FileDownloadService.DownloadRequest(serverFilePath, localPath);
        downloadRequest.setRequiresUnzip(true);
        downloadRequest.setDeleteZipAfterExtract(true);
        downloadRequest.setUnzipAtFilePath(path);

        FileDownloadService.OnDownloadStatusListener listener = new FileDownloadService.OnDownloadStatusListener() {

            @Override
            public void onDownloadStarted() {
                Log.i(OuverActivity.class.getName(), "Download started");
                resultView.setText(R.string.download_started);
            }

            @Override
            public void onDownloadCompleted() {
                Log.i(OuverActivity.class.getName(), "Download completed");
                resultView.setText(R.string.download_complete);
                customInitModel();
            }

            @Override
            public void onDownloadFailed() {
                setErrorState("Failed to download model");
                resultView.setText(R.string.failed_to_download_model);
            }

            @Override
            public void onDownloadProgress(int progress) {

                String progresspercentage = String.valueOf(progress);
                String downloadpercentage = progresspercentage + "% downloaded";
                resultView.setText(downloadpercentage);
            }
        };

        FileDownloadService.FileDownloader downloader = FileDownloadService.FileDownloader.getInstance(downloadRequest, listener);
        downloader.download(this);
    }



    private void customInitModel() {
        Log.i(OuverActivity.class.getName(), "Custom init model");
        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        String lang = sharedPreferences.getString("lang", getResources().getString(R.string.en));
        String modelPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/models/" + lang;

        File file = new File(modelPath);
        if (file.exists()) {
            Executor executor = Executors.newSingleThreadExecutor();
            Handler handler = new Handler(Looper.getMainLooper());
            executor.execute(() -> {
                try {
                    Model model = new Model(modelPath);
                    handler.post(() -> {
                        this.model = model;
                        setUiState(STATE_READY);
                    });
                } catch (Exception e) {
                    handler.post(() -> setErrorState("Failed to initialize model"));
                }
            });
        } else {
            downloadModel(lang);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == PERMISSIONS_REQUEST_ALL_FILES_ACCESS) {
            if (Build.VERSION.SDK_INT >= 30) {
                if (Environment.isExternalStorageManager()) {
                    customInitModel();
                } else {
                    setErrorState("All files access permission needed");
                }
            }
        }
        if(requestCode == BluetoothState.REQUEST_CONNECT_DEVICE) {
            if(resultCode == Activity.RESULT_OK)
                bt.connect(data);
            Connectedto = "BT";
            isconnectusb = true;
            String usbconnectedwelcome;
            if (hasuser){
                usbconnectedwelcome = R.string.hi + userName + R.string.you_are_now_connected;
            }else{
                usbconnectedwelcome = getResources().getString(R.string.hey_you_are_now_connected);
            }
            sendTxtBluetooth(usbconnectedwelcome);
        } else if(requestCode == BluetoothState.REQUEST_ENABLE_BT) {
            if(resultCode == Activity.RESULT_OK) {
                bt.setupService();
                bt.startService(BluetoothState.DEVICE_ANDROID);
                /*setup();*/
                /*  } else {*/
                // Do something if user doesn't choose any device (Pressed back)
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == PERMISSIONS_REQUEST_RECORD_AUDIO) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                requestAllFilesAccessPermission();
            } else {
                setErrorState("Record audio permission needed");
            }
        } else if (requestCode == PERMISSIONS_REQUEST_ALL_FILES_ACCESS) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                customInitModel();
            } else {
                setErrorState("All files access permission needed");
            }
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        if (speechService != null) {
            speechService.stop();
            speechService.shutdown();
            soundsensorMODE = false;
        }

        if (speechStreamService != null) {
            speechStreamService.stop();
            soundsensorMODE = false;
        }
    }

    @Override
    public void onResult(String hypothesis) {
        try {
            JSONObject objc = new JSONObject(hypothesis);
           /*txtWorkToSend(obj.getString("text"));*/


        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onFinalResult(String hypothesis) {
        try {
            JSONObject obj = new JSONObject(hypothesis);

            istosend = true;
            myNameAC(obj.getString("text"));

        } catch (JSONException e) {
            e.printStackTrace();
        }
        if (speechStreamService != null) {
            speechStreamService = null;
        }


    }

    @Override
    public void onPartialResult(String hypothesis) {

        try {
        JSONObject obj = new JSONObject(hypothesis);
            istosend = false;
            myNameAC(obj.getString("partial"));


        } catch (JSONException e) {
            e.printStackTrace();
        }

    }

    @Override
    public void onError(Exception e) {
        setErrorState(e.getMessage());
    }

    @Override
    public void onTimeout() {
        setUiState(STATE_DONE);
    }

    private void setUiState(int state) {
        switch (state) {
            case STATE_START:
                if (hasuser){
                    String txt1 = getString(R.string.hi) + " "  + userName + "," + getString(R.string.preparing);
                    resultView.setText(txt1);
                }else{
                    resultView.setText(R.string.preparing);
                }
                ((Button) findViewById(R.id.soundsensorBT)).setText(R.string.soundsensor);
                resultView.setMovementMethod(new ScrollingMovementMethod());
                findViewById(R.id.soundsensorBT).setEnabled(false);
                findViewById(R.id.recognize_mic).setEnabled(false);
                findViewById(R.id.pause).setEnabled((false));
                break;
            case STATE_READY:
                String txt2 = getString(R.string.hi) + " "  + userName + "," + getString(R.string.ready);
                resultView.setText(txt2);
                ((Button) findViewById(R.id.recognize_mic)).setText(R.string.recognize_microphone);
                ((Button) findViewById(R.id.soundsensorBT)).setText(R.string.soundsensor);
                findViewById(R.id.soundsensorBT).setEnabled(true);
                findViewById(R.id.recognize_mic).setEnabled(true);
                findViewById(R.id.pause).setEnabled(false);
                break;
            case STATE_DONE:
                String txt3 = getString(R.string.hi) + " " + userName + "," + getString(R.string.ready);
                resultView.setText(txt3);
                ((Button) findViewById(R.id.soundsensorBT)).setText(R.string.soundsensor);
                ((Button) findViewById(R.id.recognize_mic)).setText(R.string.recognize_microphone);
                findViewById(R.id.soundsensorBT).setEnabled(true);
                findViewById(R.id.recognize_mic).setEnabled(true);
                findViewById(R.id.pause).setEnabled(false);
                findViewById(R.id.callmeBT).setEnabled(true);
                break;
            case STATE_FILE:
                ((Button) findViewById(R.id.soundsensorBT)).setText(R.string.soundsensor_stop);
                resultView.setText(getString(R.string.listening_message));
                resultView.setText(getString(R.string.starting));
                findViewById(R.id.recognize_mic).setEnabled(false);
                findViewById(R.id.soundsensorBT).setEnabled(false);
                findViewById(R.id.pause).setEnabled(false);
                findViewById(R.id.callmeBT).setEnabled(false);
                break;

            case STATE_MIC:
                ((Button) findViewById(R.id.recognize_mic)).setText(R.string.stop_microphone);
                ((Button) findViewById(R.id.soundsensorBT)).setText(R.string.soundsensor);
                resultView.setText(getString(R.string.say_something));
                findViewById(R.id.soundsensorBT).setEnabled(false);
                findViewById(R.id.recognize_mic).setEnabled(true);
                findViewById(R.id.pause).setEnabled(true);
                break;

            case STATE_GET_NAME:
                findViewById(R.id.subtitlesLL).setVisibility(View.VISIBLE);
                findViewById(R.id.getnameLL).setVisibility(View.GONE);
                break;

            case STATE_NAME_MODE:
                ((Button) findViewById(R.id.recognize_mic)).setText(R.string.stop_microphone);
                String nameMessage = getString(R.string.waiting_for_name)+ ": " + userName ;
                resultView.setText( nameMessage );
                findViewById(R.id.soundsensorBT).setEnabled(false);
                findViewById(R.id.recognize_mic).setEnabled(true);
                findViewById(R.id.pause).setEnabled(true);
                findViewById(R.id.callmeBT).setEnabled(false);
                break;


            default:
                throw new IllegalStateException("Unexpected value: " + state);
        }
    }

    private void setErrorState(String message) {
        resultView.setText(message);
        ((Button) findViewById(R.id.recognize_mic)).setText(R.string.recognize_microphone);
        findViewById(R.id.soundsensorBT).setEnabled(false);
        findViewById(R.id.recognize_mic).setEnabled(false);
    }

    private void recognizeFile() {
        if (speechStreamService != null) {
            speechStreamService.stop();
            speechStreamService = null;
            setUiState(STATE_DONE);
        } else {
            setUiState(STATE_FILE);
            try {
                Recognizer rec = new Recognizer(model, 16000.f, "[\"one zero zero zero one\", " +
                        "\"oh zero one two three four five six seven eight nine\", \"[unk]\"]");

                InputStream ais = getAssets().open(
                        "10001-90210-01803.wav");
                if (ais.skip(44) != 44) throw new IOException("File too short");

                speechStreamService = new SpeechStreamService(rec, ais, 16000);
                speechStreamService.start(this);
            } catch (IOException e) {
                setErrorState(e.getMessage());
            }
        }
    }

    private void recognizeMicrophone() {
        if (speechService != null) {
            mynameMODE= false;
            soundsensorMODE = false;
            soundsensorcount = 0;
            setUiState(STATE_DONE);
            resultView.setText("");
            sendToConnected("");
            speechService.stop();
            speechService = null;
        } else {
            setUiState(STATE_MIC);
            try {
                Recognizer rec = new Recognizer(model, 16000.0f);
                speechService = new SpeechService(rec, 16000.0f);
                speechService.startListening(this);
            } catch (IOException e) {
                setErrorState(e.getMessage());
            }
        }
    }


    private void pause(boolean checked) {
        soundsensorMODE = false;
        if (speechService != null) {
            speechService.setPause(checked);
        }
    }
    /*-------------------------------USB-------------------------------*/

    private void startUsb() {
        if ( !isconnectusb){
        mPhysicaloid.open(); // default 9600bps

Button usbbtn = findViewById(R.id.usbconnectBT);
usbbtn.setText(R.string.usb_connected);
            Connectedto = "USB";
            isconnectusb = true;
            String usbconnectedwelcome;
            if (hasuser){
                 usbconnectedwelcome = "Hi " + userName + "you are now connected";
            }else{
                 usbconnectedwelcome = "Hi, you are now connected";
            }

            sendTxtUsb(usbconnectedwelcome);
        } else {
            mPhysicaloid.close();

            Button usbbtn = findViewById(R.id.usbconnectBT);
            usbbtn.setText(R.string.usb_connect);
            Connectedto = "";
            isconnectusb = false;
        }


    }

    private void sendTxtUsb(String str) {


            str += "/n";
            byte[] buf = str.getBytes();


            mPhysicaloid.write(buf, buf.length);



    }

    /*-------------------------------Bluetooth-------------------------------*/


private void  startBT(){
    if ( !isconnectbt){
        startBluetooth();

    } else {
        bt.stopService();
        isconnectbt = false;

    }
}




        private void startBluetooth() {


        bt.startService(BluetoothState.DEVICE_OTHER);
        Intent intent = new Intent(getApplicationContext(), DeviceList.class);
        startActivityForResult(intent, BluetoothState.REQUEST_CONNECT_DEVICE);
    }


    private void sendTxtBluetooth(String str) {

        str += "/n";
        bt.send(str, true);


    }
    /*-------------------------------Menu-------------------------------*/
    private void settingsAC() {


        if (findViewById(R.id.settingsLL).getVisibility() == View.GONE){
            findViewById(R.id.settingsLL).setVisibility(View.VISIBLE);
        } else {
            findViewById(R.id.settingsLL).setVisibility(View.GONE);
        }
    }

    private void callAC() {


        if (findViewById(R.id.callLL).getVisibility() == View.GONE){
            findViewById(R.id.callLL).setVisibility(View.VISIBLE);
        } else {
            findViewById(R.id.callLL).setVisibility(View.GONE);
        }
    }

    public void languagesAC(){
        startActivity(new Intent(this, SettingsActivity.class));
    }
    /*------------------------------------Send to connected -----------------------------*/

    private void sendToConnected(String txt){
        if (Connectedto == "USB") {
            sendTxtUsb(txt);
        } else if (Connectedto == "BT") {
                sendTxtBluetooth(txt);
            }

    }
    /*------------------------------------Work txt size to be sent -----------------------------*/

    private void txtWorkToSend (String txt){
if (istosend){
    deletetimer("final");
    if(soundsensorMODE){
        soundsensorRestarter("final");
    }
}else{
    if(txt != null && !txt.trim().isEmpty()){
        deletetimer("temp");
        if(soundsensorMODE){
            soundsensorRestarter("temp");
        }
    }else{
        deletetimer("final");
        if(soundsensorMODE){
            soundsensorRestarter("final");
        }
    }
}



/*por texto maior que 1*/
        if(txt != null && !txt.trim().isEmpty())

            if (txt != txtPreRecieved) {
                txtPreRecieved = txt;


                int txtLenght = txt.length();


                if (txtLenght>62)
                    while (txtLenght>62)  {
                        /*get first 61 characters*/
                        String cuttedTxtSubstring = txt.substring(0, 61);

                        /*divide text into list of words dividing them in each space*/
                        String[] cuttedText = cuttedTxtSubstring.split("\\s+");

                        /*get last word of string*/
                        String lastword = cuttedText[cuttedText.length - 1];

                        /*get length of last word*/
                        int lastwordsize = lastword.length();

                        char lastletter =cuttedTxtSubstring.charAt(60);


                        if (lastletter == ' '){
                            lastwordsize +=1;
                        }

                        /*get beguinning of extended text*/
                        int inicio = 61 - lastwordsize;



                        /*set text to extended only*/
                        txt = txt.substring(inicio, txtLenght);
                        txtLenght = txt.length();


                    }

                resultView.setText(txt);
                sendToConnected(txt);


            }
    }

    public void deletetimer (String count){

        if (count == "final"){
            counterfinal +=1;
        } else{
            if (counterfinal != 0){
            counterfinal = 0;}
        }

        if (counterfinal == 20){
            if(resultView.getText()!="") {
                resultView.setText("");
                sendToConnected("");
            }

    }

    }

    public void soundsensorRestarter (String count){

        if (count == "final"){
            soundsensorcount +=1;
        } else{
            if (soundsensorcount != 0){
                soundsensorcount = 0;}
        }


        if (soundsensorcount == 80){
            recognizeMicrophone();
            soundsensorcount = 0;

                soundsensorAC();


        }
    }



    /*------------------------------------Notifications-----------------------------*/

    private BroadcastReceiver onNotice= new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String pack = intent.getStringExtra("package");
            String title = intent.getStringExtra("title");
            String text = intent.getStringExtra("text");


            PackageManager packageManager= getApplicationContext().getPackageManager();
            try {
                appName = (String) packageManager.getApplicationLabel(packageManager.getApplicationInfo(pack, PackageManager.GET_META_DATA));
            } catch (PackageManager.NameNotFoundException e) {
                e.printStackTrace();
            }

            if (notificationdstatus){

                String txt = appName + ": " + title + " " + text;


                resultView.setText(txt);
                sendToConnected(txt);
                deletetimer("final");


                }


            }



    };

    public void notificationsclicked() {
        if (notificationdstatus){
            notificationdstatus = false;
            Button notificationsbtn = findViewById(R.id.notificationsBT);
            notificationsbtn.setText(R.string.notifications_off);

        }else{
            notificationdstatus = true;
            Button notificationsbtn = findViewById(R.id.notificationsBT);
            notificationsbtn.setText(R.string.notifications_on);
            Toast.makeText(getApplicationContext(),R.string.notifications_warning,Toast.LENGTH_SHORT).show();
        }

    }

    /*------------------------------------Sound Sensor-----------------------------*/


    public void soundsensorAC() {

        try {
            soundMeter.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
        soundsensorMODE = true;
        float volumenow = soundMeter.getAmplitude();
        while (volumenow < 80) {
            volumenow = soundMeter.getAmplitude();
            try {
                Thread.sleep(250);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

        }
        soundMeter.stop();
        recognizeMicrophone();
    }
    /*------------------------------------Get Name-----------------------------*/

    public  void getnameclicked(){
        TextView name;
        name = (TextView) findViewById(R.id.editTextTextPersonName);
        sharedpreferences = getSharedPreferences(mypreference,
                Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedpreferences.edit();
        if(sharedpreferences.contains(Name)){
            editor.remove("Name");
            editor.commit();
        }
        String n = name.getText().toString();
        userName = n;
        hasuser = true;
        editor.putString(Name, n);
        editor.commit();
        setUiState(STATE_GET_NAME);
    }

    public void changenameagainclicked() {
        findViewById(R.id.subtitlesLL).setVisibility(View.GONE);
        findViewById(R.id.getnameLL).setVisibility(View.VISIBLE);

    }
    /*------------------------------------My name-----------------------------*/

    public void myNameAC(String txt){
        if(mynameMODE){
            if(txt.contains(userName)){
                mynameMODE= false;
                txtWorkToSend(txt);
            }
            }else{
            txtWorkToSend(txt);
        }
    }
public void callmeAC() {
    if ((userName != null && !userName.trim().isEmpty())) {
        mynameMODE = true;
        recognizeMicrophone();
        setUiState(STATE_NAME_MODE);
    } else{
        Toast.makeText(getApplicationContext() , R.string.set_name_warning, Toast.LENGTH_SHORT).show();
    }
}

public void playwarningAC(){
    String toSpeak = getString(R.string.warning_message);
    t1.speak(toSpeak, TextToSpeech.QUEUE_FLUSH, null);
}




    /*------------------------------------End-----------------------------*/
    }







