#if defined(ESP32)
  #include "WiFi.h"
#elif defined(ESP8266)
  #include "ESP8266WiFi.h"
#endif

#include "PubSubClient.h"

// WiFi Credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PWD "YOUR_WIFI_PASSWORD"

// MQTT Configuration
#define MQTT_PORT 1883
#define MQTT_SERVER "IoT.ScheduleCare.ca"
#define MQTT_USER "EMBED"
#define MQTT_PWD "YOUR_MQTT_PASSWORD"

// My topic
const String rootTopic = "EMBED";
const String studentName = "Matheus Ferraro";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// --- MQTT Functions ---

bool mqttConnect() {
  if (mqttClient.connected()) return true;

  Serial.print("Connecting MQTT, Server=");
  Serial.println(MQTT_SERVER);


  mqttClient.setCallback(mqttCallback);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
  // Unique ClientID using MAC address (Good practice for Portfolios!)
  String clientID = "NBCC-" + String(WiFi.macAddress());
  Serial.printf("ClientID %s connecting to MQTT broker...\n", clientID.c_str());

  if (mqttClient.connect(clientID.c_str(), MQTT_USER, MQTT_PWD)) {
    Serial.println("MQTT broker connected");

    String subscribeTopic = rootTopic + "/" + studentName + "/relay/#";
    mqttClient.subscribe(subscribeTopic.c_str());

    return true;
  } else {
    Serial.print("MQTT connect failed, state: ");
    Serial.println(mqttClient.state());
    return false;
  }
}



void mqttCallback(char * topic, uint8_t buffer, unsigned int length){
  String value;
  value.reserve(length + 1);
  for (int i = 0; i < length; i++)
    value.concat((char)buffer[i]);
  Serial.println(String("MQTT:callback:t=") + topic + ", v=" + value);
}


// --- WiFi Functions ---

String getEncryptionType(int i) {
  int encType = WiFi.encryptionType(i);
  switch (encType) {
#if defined(ESP32)
    case WIFI_AUTH_OPEN: return "open";
    case WIFI_AUTH_WPA_PSK: return "WPA";
    case WIFI_AUTH_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA+WPA2";
    case WIFI_AUTH_WPA3_PSK: return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2+WPA3";
#elif defined(ESP8266)
    case ENC_TYPE_NONE: return "NONE";
    case ENC_TYPE_TKIP: return "WPA/PSK";
    case ENC_TYPE_CCMP: return "WPA2/PSK";
#endif
    default: return "Other";
  }
}

void wifiScanner() {
  Serial.println("\nWiFi Scanner starting...");
  int n = WiFi.scanNetworks(false, true);
  
  if (n == 0) {
    Serial.println("No WiFi networks found");
  } else {
    Serial.printf("%d networks found:\n", n);
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s (Ch:%d, RSSI:%d) Enc:%s\n", 
                    i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), getEncryptionType(i).c_str());
    }
  }
  WiFi.scanDelete();
}

bool wifiConnect() {
  if (WiFi.status() == WL_CONNECTED) return true;

  Serial.print("Connecting to SSID: "); Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PWD);

  for (int i = 0; i < 20; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(" - Connected!");
      Serial.print("IP: "); Serial.println(WiFi.localIP());
      return true;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println(" FAILED");
  return false;
}

// --- Main Program ---

void setup() {
  // Using 115200 is more stable for ESP32 WiFi/MQTT tasks
  Serial.begin(115200); 
  delay(1000); // Small delay to let serial stabilize
  
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  
  wifiScanner();
}

void loop() {
  if (wifiConnect()) {
    if (mqttConnect()) {

      mqttClient.loop();

      static int publishCount = 0;
      mqttClient.publish(
      (rootTopic + "/" + studentName).c_str(),
      String(++publishCount).c_str(),
      true); // turn ON the ‘retain’ flag
      Serial.print("publish name:");
      Serial.println(publishCount);
    }
    if(strcmp(topic, "/relay") == 0){
      Serial.println(String("RELAY Control=") + value);
      if(value.equals("ON"))
        relay.turnOn();
      else if (value.equals("OFF"))
        relay.turnOff();
      else
        Serial.println("RELAY Control:unrecognized");
    }
    delay(3000);
  }
  
}