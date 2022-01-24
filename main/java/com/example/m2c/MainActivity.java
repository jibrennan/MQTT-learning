package com.example.m2c;

//import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import org.eclipse.paho.android.service.MqttAndroidClient;
//import org.eclipse.paho.client.mqttv3.DisconnectedBufferOptions;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttClient;
//import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

//import android.content.DialogInterface;
import android.graphics.Color;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Bundle;
//import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.os.Handler;
import java.util.Calendar;
//import java.util.Date;

public class MainActivity extends AppCompatActivity {
    //MQTT client items
    final String serverURI = "tcp://10.0.0.0:1883";
    final String clientId = MqttClient.generateClientId();
    final String subscriptionTopic = "DSC";
    MqttAndroidClient mqttAndroidClient = new MqttAndroidClient(MainActivity.this, serverURI, clientId);

    final Handler handler = new Handler();
    //storage for any variables and the data on the screen
    CharSequence data;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final Button button = findViewById(R.id.button1);
        final TextView text1 = findViewById(R.id.textView1);
        // ringtone items
        Uri notification = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_RINGTONE);  // I want to be able to use whatever is set as the current ringtone.
        Ringtone r = RingtoneManager.getRingtone(getApplicationContext(), notification);

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try {
                    IMqttToken token = mqttAndroidClient.connect();
                    token.setActionCallback(new IMqttActionListener() {
                        @Override
                        public void onSuccess(IMqttToken asyncActionToken) {
                            // We are connected
                            Toast.makeText(MainActivity.this, "Connected!", Toast.LENGTH_SHORT).show();
                            view.setBackgroundColor(Color.GREEN);
                            try {
                                mqttAndroidClient.subscribe(subscriptionTopic, 0, null, new IMqttActionListener() {
                                    @Override
                                    public void onSuccess(IMqttToken asyncActionToken) {
                                        Toast.makeText(MainActivity.this, "Subscribed!", Toast.LENGTH_SHORT).show();
                                        text1.setText("Waiting...");
                                        data = text1.getText();
                                        button.setText("Subscribed");
                                    }
                                    @Override
                                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                                        Toast.makeText(MainActivity.this, "Failed to Subscribe!", Toast.LENGTH_SHORT).show();
                                    }
                                });
                            } catch (MqttException e) {
                                e.printStackTrace();
                            }
                        }
                        @Override
                        public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                            // Something went wrong e.g. connection timeout or firewall problems
                            Toast.makeText(MainActivity.this, "Failed!", Toast.LENGTH_SHORT).show();
                        }
                    });
                } catch (MqttException e) {
                    e.printStackTrace();
                }
            }
        });

        mqttAndroidClient.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean reconnect, String serverURI) {
                if (reconnect) {
                    Toast.makeText(MainActivity.this, "Reconnecting...", Toast.LENGTH_SHORT).show();
                    // Because Clean Session is true, we need to re-subscribe
                } else {
                    Toast.makeText(MainActivity.this, "Connected!", Toast.LENGTH_SHORT).show();
                }
            }
            @Override
            public void connectionLost(Throwable cause) {
                Toast.makeText(MainActivity.this, "Connection Lost!", Toast.LENGTH_SHORT).show();
            }
            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {
            }
            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                text1.setText(new String(message.getPayload()));
                data = text1.getText();
                if ((Calendar.HOUR_OF_DAY >= 7) && (Calendar.HOUR_OF_DAY <= 20)) {
                    try {
                        r.play();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                    handler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            // Do something after 5s = 5000ms
                            text1.setText("Waiting...");
                            data = text1.getText();
                            r.stop();
                        }
                    }, 2000);  // MQTT board resets after 3 seconds so just keep it under 3 seconds.
                }
        });

    }
   // learned a thing or two here... save all of the data when the state changes (rotate, sleep, switching apps)
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putCharSequence("textbox", data);
    }
}