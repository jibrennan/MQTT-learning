# MQTT-learning
That time I explored the basic functionality of MQTT messaging using the absolute easiest ways possible. 

This goal of this project was to learn a little bit about using MQTT and to get something basic working with the components I have laying around. Most of this code is referenced from stock examples and the entire project was completed in one Sunday afternoon.

The wife wanted a way to be able to hear the alarm panel chime while she was upstairs. The security panel is a DSC1616 which connects AUX2 to ground every time the door chime activates. To get the input from the panel to the Arduino Uno, I used a 12vdc to 5vdc optocoupler from Amazon. By feeding the input to the optocoupler from the Uno and looking for a LOW when the chime activates, the Uno then sends the Huzzah ESP8266 board a simple 3 byte serial command. 

When the Huzzah receives the command, it publishes a CHIME message to the "DSC" topic on a Mosquitto MQTT server running on a Pi Zero W. 

There are two Android phones that run the M2C app located upstairs. The phones are old and only run this app on Wi-Fi. They are both subscribed to the "DSC" topic and when they receive any message from the topic, they play the user defined RINGTONE for 2 seconds. 

Looks like garbage, works like a champ. 
