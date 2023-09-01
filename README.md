# MQTTClientESP32
A simple project for testing MQTT-communication between an ESP-32 board and an MQTT-broker using the free cloud-option from EMQX. 

The board goes to sleep with a wake up timer, and when it wakes up it sends a payload to the broker and goes back to sleep.

It then keeps track of the successrate of each payload using these variables that are included in the payload.

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int mqttSuccesses = 0;
RTC_DATA_ATTR int mqttFailed = 0;


The following lines need to be updated with your personal configuration:

const char *ssid = "YOUR_SSID";           // Enter your WiFi name
const char *password = "YOUR_PASSWORD"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "YOURBROKER.emqxsl.com"; // broker address
const char *mqtt_username = "YOUR_USERNAME";                            // username for authentication
const char *mqtt_password = "YOUR_PASSWORD";                            // password for authentication

It is currently set up to use a TTGO 2.1.6 board, if you use a different board, you might also have to change that in the PlatformIO-settings.

This project is based on this example from EMQX:

https://github.com/emqx/MQTT-Client-Examples/blob/master/mqtt-client-ESP32/esp32_connect_mqtt_via_tls.ino

The CA certificate is the one used there. 

The big changes are:

 - Wrapped it in a PlatformIO-project
 - Removed the code that subscribes the client to a topic
 - Added a sleep-timer
 - Added counter for how many times the board wakes up from sleep
 - Added counter for how many times the client successfully publishes a payload to the broker.
 - Added counter for how many times the client unsuccessfully publishes a payload to the broker.
 - Changed the payload to include these counters
