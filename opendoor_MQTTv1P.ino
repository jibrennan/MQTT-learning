/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - (JB) Publishes "hello" to the "DSC" group. 
  - (JB) Listens for a serial message from the Uno indicating that a chime has been activated.
  - (JB) Publishes a "chime" message on the DSC group. 
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

(JB) This was modified from it's original state as an example accessible from within the Arduino IDE.  
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

byte doorChime []       = {0x02, 0x43, 0x03}; //[stx] C [etx] will come from the Uno when the chime is activated. 
const char* ssid        = "MY_SSID";
const char* password    = "MY_PASSWORD";
const char* mqtt_server = "MY_SERVER_IP_ADDRESS";  //just the IP address only, nothing else required, separated by dots, not commas. 
char panelMSG [3]; // buffer for messages from the control panel arduino
#define MSG_BUFFER_SIZE (50)    
char msg[MSG_BUFFER_SIZE]; 
int i = 0; // this is a loop counter
int k = 0; // this is used as a flag.

WiFiClient espClient;           // create instance of the wifi client
PubSubClient client(espClient); // create the instance of the MQTT client

//---------------------------------------------------------------------------------
//
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println(); // logging
  Serial.print("Connecting to "); // these logging message were helpful when troubleshooting 
  Serial.println(ssid); // logging
  WiFi.mode(WIFI_STA);  // means that we'll be connecting to an access point
  WiFi.begin(ssid, password); // connect usng the wifi ssid and password

  while (WiFi.status() != WL_CONNECTED) {  //logging while connecting, or not connected. 
    delay(500);
    Serial.print("."); 
  }
  randomSeed(micros()); // not sure why this random timer is needed.  
  // print the info to the serial console for troubleshooting. 
  Serial.println("");                //logging 
  Serial.println("WiFi connected");  //logging
  Serial.println("IP address: ");    //logging
  Serial.println(WiFi.localIP());    //logging
}
//----------------------------------------------------------------------------------
//when data is received, this callback will handle it. This board doesnt not actually do anything with topic messages it receives. 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived ["); 
  Serial.print(topic);               
  Serial.print("] ");                
  for (int j = 0; j < length; j++) { 
    Serial.print((char)payload[j]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}
//-------------------------------------------------------------------------------------
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("DSC", "hello");  // DSC is the topic for the DSC panel, let everyone now you've just connected. 
      // ... and resubscribe
      //client.subscribe("inTopic"); // we don't need this for this project. 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//--------------------------------------------------------------------------------------
// set the built-in LED to an output, fire up the serial port, connect wifi, register with the MQTT server, register your callbacks
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883); 
  client.setCallback(callback); // register the callback to handle incoming messages.  
}
//-------------------------------------------------------------------------------------
// first check to see if we have a valid message, then compare the message, if it's a chime "c" then we want to publish the "CHIME" message. 
void processData(){
  if (panelMSG [2] == 0x03 ){ // [ETX] -- we are only looking for 3 byte messages so let's do a basic check to see if we've received a proper message.
      if (panelMSG [1] == 0x43){ // "c" door chime indication from the panel
        Serial.println("Chime.");  // logging
        snprintf (msg, MSG_BUFFER_SIZE, "CHIME #%ld");  //construct the message - in this example, this board is the only component that publishes anything to the topic, the other clients just receive. 
        client.publish("DSC", msg); // actually publish the message
      }
      else {
        Serial.println("Incorrect Format Received.");  //I got something but it was not what I wanted. 
      }
  }
}
//------------------------------------------------------------------------------------
// while the client is connected, constantly check the serial buffer for data
void loop() {
  if (!client.connected()) {
    reconnect(); // if we're not connected, then try to reconnect. 
  }
  client.loop();
  
  if (Serial.available() >0){ // these are variable lengh responses with no eol or STX/ETX formatting, so we're going to read one byte at a time until the buffer is empty...  
    delay(50); // wait for the data to be complete, you don't want to start doing stuff too early. 
    Serial.println("data received.");
    i=0; // reset the counter just in case. 
    while (Serial.available()>0){
      char inByte = Serial.read();  // reads one byte at a time
      panelMSG[i] = (inByte);       // take each byte and stick them into the panelMSG array
      i++;
    }
    k = 1; // just a flag to let us know that we got something.  
  }
  if (k == 1){ // if the flag has been set... 
    Serial.println("sending data to process.");
    processData();  // call the process data function to take a look at what we have
    k=0; // reset our flag
    i=0; // reset our counter
  }
}
