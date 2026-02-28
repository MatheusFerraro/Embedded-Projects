#include <EEPROM.h>

typedef struct{
  char Wifi_SSID[16];
  char Wifi_Password[16];

  void setSSID(const char * s)
  { strcpy(Wifi_SSID, s); }
  void setSSID(const String & s)
  { setSSID(s.c_str()); }

  void setPassword(const char * s)
  { strcpy(Wifi_Password, s); }
  void setPassword(const String & s)
  { setPassword(s.c_str()); }

} ConfigData;

// CONFIG_SIZE defines how many EEPROM bytes are needed
//
#define CONFIG_SIZE (sizeof(ConfigData))
// because we might have multiple uses for EPPROM memory,
// it is good to define the starting point for the
// Config block
//
#define CONFIG_START 0
ConfigData Data;





void configLoad() {
  Serial.println("Config LOAD");
  // copy the contents of EEPROM into our Config struct
  char * p = (char*) &Data;
  for (int i=0; i < CONFIG_SIZE; i++)
  p[i] = EEPROM.read(CONFIG_START + i);
  }

  void configClear() {
  Serial.println("Config CLEAR");
  for (int i=0; i<CONFIG_SIZE; i++)
  EEPROM.write(CONFIG_START + i, -1);
  #if defined(ESP32) || defined(ESP8266)
  EEPROM.commit();
  #endif
}



void configSave() {
  Serial.println("Config SAVE");
  char * p = (char *) &Data;
  for (int i=0; i<CONFIG_SIZE; i++)
  EEPROM.write(CONFIG_START + i, p[i]);
  #if defined(ESP32) || defined(ESP8266)
  EEPROM.commit();
  #endif
}

void configDump() {
  Serial.println("Config dump:");
  Serial.print("  Size:    "); Serial.println(CONFIG_SIZE);
  Serial.print("  SSID:    "); Serial.println(Data.Wifi_SSID);
  Serial.print("  PWD:     "); Serial.println(Data.Wifi_Password);
  Serial.println();
}

String readLine(const char* prompt) {
  String result;
  Serial.print(prompt);
  while (true) {
  // wait for a character
  while (!Serial.available())
  ;
  char c = Serial.read();
  if (c == '\n') {
  Serial.println(result);
  return result;
  }
  result += c;
  }
}




void setup() {

  #if defined(ESP32)
  Serial.begin(115200);
  #elif defined(ESP8266)
  Serial.begin(74880);
  #else
  Serial.begin(9600);
  #endif
  #if defined(ESP32) || defined(ESP8266)
  EEPROM.begin(CONFIG_SIZE);
  #endif
  Serial.println();
  Serial.println("Hello");
  configLoad();

}

void loop() {
  configDump();
  String cmd = readLine("Load/Clear/SSID/Password/saVe ? ");
  cmd.toUpperCase();
  if (cmd.equals("L"))
  configLoad();
  else if (cmd.equals("C"))
  configClear();
  else if (cmd.equals("S"))
  Data.setSSID(readLine("SSID > "));
  else if (cmd.equals("P"))
  Data.setPassword(readLine("Password > "));
  else if (cmd.equals("V"))
  configSave();

}
