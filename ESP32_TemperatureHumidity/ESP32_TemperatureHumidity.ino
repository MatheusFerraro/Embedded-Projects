#if defined(ESP32)
  #include "WiFi.h"
#elif defined(ESP8266)
  #include "ESP8266WiFi.h"
#endif

#include "PubSubClient.h"
#include <DHT.h>
#include "time.h"  //Required for NTP Time

// --- Hardware Configuration ---
#define DHTPIN 4      // GPIO 4 (D2)
#define DHTTYPE DHT11 // DHT 11
#define LED_PIN 2     // GPIO 2 (D4)

// WiFi Credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PWD "YOUR_WIFI_PASSWORD"

// MQTT Configuration
#define MQTT_PORT 1883
#define MQTT_SERVER "IoT.ScheduleCare.ca"
#define MQTT_USER "EMBED"
#define MQTT_PWD "YOUR_MQQT_PASSWORD"

// My topic
const String rootTopic = "EMBED";
const String studentName = "Matheus Ferraro";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
DHT dht(DHTPIN, DHTTYPE); //Input Peripheral

// --- Global Variables ---
unsigned long lastMsgTime = 0;
unsigned long publishInterval = 1000; //Start at 1 per second
bool mqttLogEnabled = true;           //Enable/Disable flag

// NTP Server settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -14400;   // Adjust foR timezone (e.g., -4h for Atlantic)
const int   daylightOffset_sec = 3600;

// --- Logger Class ---
class Logger {
  public:
    void log(String msg) {
      Serial.print("[LOG] ");
      Serial.println(msg);
      //  Publish diagnostic if connected & enabled
      if (mqttLogEnabled && mqttClient.connected()) {
        String debugTopic = rootTopic + "/" + studentName + "/debug";
        mqttClient.publish(debugTopic.c_str(), msg.c_str());
      }
    }
};
Logger myLogger;

// --- Helper: Get Time ---
String getFormattedTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "Time Error";
  }
  char timeStringBuff[50];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeStringBuff);
}

// --- MQTT Functions ---

// 1. Callback (Modified to handle commands) 
// Corrected signature: buffer is now a pointer (uint8_t *)
void mqttCallback(char * topic, uint8_t * buffer, unsigned int length) {
  String value;
  value.reserve(length + 1);
  
  // Now buffer[i] works because buffer is an array pointer
  for (int i = 0; i < length; i++) {
    value.concat((char)buffer[i]);
  }
  
  String topicStr = String(topic);
  myLogger.log("Msg received: " + topicStr + " val=" + value);
  
  // A. Control LED
  if (topicStr.endsWith("/relay")) {
      if(value.equals("ON")) {
        digitalWrite(LED_PIN, HIGH);
        myLogger.log("LED ON");
      }
      else if (value.equals("OFF")) {
        digitalWrite(LED_PIN, LOW);
        myLogger.log("LED OFF");
      }
  }
  // B. Control Publish Rate
  else if (topicStr.endsWith("/rate")) {
      long newRate = value.toInt();
      if (newRate > 0) {
        publishInterval = newRate;
        myLogger.log("Rate updated to: " + String(publishInterval));
      }
  }
  // C. Enable/Disable Diagnostics
  else if (topicStr.endsWith("/diag")) {
      if (value.equals("ON")) mqttLogEnabled = true;
      else if (value.equals("OFF")) mqttLogEnabled = false;
  }
}

bool mqttConnect() {
  if (mqttClient.connected()) return true;

  Serial.print("Connecting MQTT, Server=");
  Serial.println(MQTT_SERVER);
  
  mqttClient.setCallback(mqttCallback); // Ensure callback is set
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
  String clientID = "NBCC-" + String(WiFi.macAddress());
  
  if (mqttClient.connect(clientID.c_str(), MQTT_USER, MQTT_PWD)) {
    Serial.println("MQTT broker connected");
    
    //Subscribe to specific sub-topics to avoid loops
    String baseTopic = rootTopic + "/" + studentName;
    mqttClient.subscribe((baseTopic + "/relay").c_str());
    mqttClient.subscribe((baseTopic + "/rate").c_str());
    mqttClient.subscribe((baseTopic + "/diag").c_str());

    //Publish Hello Message with Time
    String helloMsg = "Hello! Connected at: " + getFormattedTime();
    mqttClient.publish((baseTopic + "/hello").c_str(), helloMsg.c_str());

    return true;
  } else {
    Serial.print("MQTT connect failed, state: ");
    Serial.println(mqttClient.state());
    return false;
  }
}

// --- WiFi Functions (Unchanged) ---

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
      
      //Initialize Time after WiFi connects
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      
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
  Serial.begin(115200); 
  delay(1000); 
  
  // Hardware Init
  pinMode(LED_PIN, OUTPUT);
  dht.begin();
  
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  wifiScanner();
}

void loop() {
  if (wifiConnect()) {
    if (mqttConnect()) {
      mqttClient.loop();

      //Non-blocking timer using 'publishInterval' variable
      unsigned long now = millis();
      if (now - lastMsgTime > publishInterval) {
        lastMsgTime = now;

        // Read Sensor
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        // Create JSON 
        String json = "{\"temp\":" + String(t) + ",\"hum\":" + String(h) + "}";
        
        mqttClient.publish(
          (rootTopic + "/" + studentName + "/data").c_str(),
          json.c_str(),
          true
        );
        Serial.println("Published: " + json);
      }
    }
  }
}