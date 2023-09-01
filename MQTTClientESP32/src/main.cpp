#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// WiFi
const char *ssid = "YOUR_SSID";           // Enter your WiFi name
const char *password = "YOUR_PASSWORD"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "YOURBROKER.emqxsl.com"; // broker address
const char *topic = "esp32/test";                              // define topic
const char *mqtt_username = "YOUR_USERNAME";                            // username for authentication
const char *mqtt_password = "YOUR_PASSWORD";                            // password for authentication
const int mqtt_port = 8883;                                    // port of MQTT over TLS/SSL

// Sleep Timers
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 60       /* Time ESP32 will go to sleep (in seconds) */

// load DigiCert Global Root CA ca_cert
const char *ca_cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
    "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
    "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
    "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4="
    "-----END CERTIFICATE-----\n";

// init secure wifi client
WiFiClientSecure espClient;
// use wifi client to init mqtt client
PubSubClient client(espClient);

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int mqttSuccesses = 0;
RTC_DATA_ATTR int mqttFailed = 0;

char mqttPayload[100];


void setup()
{
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println();
  Serial.println("waking up...");
  bootCount += 1;
  // Set software serial baud to 115200;
  Serial.begin(9600);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected to the WiFi network");

  // set root ca cert
  espClient.setCACert(ca_cert);
  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  String client_id= "esp32-client-";;
  while (!client.connected())
  {
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 5 seconds.");
      delay(5000);
    }
  }

  snprintf(mqttPayload, sizeof(mqttPayload), "{\nclientId: %s,\nbootCount: %d,\nmqttSuccesses: %d,\nmqttFailed: %d\n}", client_id.c_str(), bootCount, mqttSuccesses, mqttFailed);


  // publish and subscribe
  Serial.print("publishing message ");
  bool publishStatus = client.publish(topic, mqttPayload, true); // publish to the topic
  if (publishStatus)
  {
    Serial.println("was successfull\n");
    mqttSuccesses+=1;
  }
  else
  {
    Serial.println("was unsuccessfull\n");
    mqttFailed +=1;
  }
  Serial.println("payload: ");
  Serial.println(mqttPayload);
  esp_deep_sleep_start();
  Serial.println("Should never be printed");
}

void loop()
{
  
}

