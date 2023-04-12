#include <PubSubClient.h>
#include <PZEM004Tv30.h>
#include <ArduinoJson.h>

#if defined(ESP32)
  // ESP32 libraries
  #define PZEM_SERIAL Serial2             // Defining the Serial port
  PZEM004Tv30 pzem(PZEM_SERIAL, 16, 17);  // Assigning the pins to serial port
  #include <WiFi.h>                       // Importing the ESP32 wifi library
#elif defined(ESP8266)
  // ESP8266 libraries
  #include <SoftwareSerial.h>             // Importing the Softwarial serial library
  SoftwareSerial pzemSerial(0, 2);        // Use software serial on ESP8266
  PZEM004Tv30 pzem(pzemSerial);           // Assigning the software serial to PZEM library
  #include <ESP8266WiFi.h>                // importing the ESP8266 wifi library
#endif

const char* ssid = "AndroidAP_5321";             // WiFi SSID to connect
const char* password = "asdfghjkl";              // WiFi password to connect
const char* mqttServer = "191.268.1.222";        // MQTT Broker IP to send/recive data
const int mqttPort = 1883;                       // MQTT Broker port to send/recive data
const char* mqttUser = "";                       // MQTT username for security purpose
const char* mqttPassword = "";                   // MQTT password for security purpose
const char* mqttTopic = "ESP32-and-ESP8266";     // MQTT Topic to send data to specific topic
const char* mqttClientID = "Meter-1";            // Device/Meter name which reading it should'nt be same for two devices

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Connect_WiFi_and_MQTT();
  Serial.begin(115200);
}

void loop() {
  ConnectionCheck();         //Used to check the MQTT Connection
  DynamicJsonDocument json = ReadData();
  String jsonString;
  serializeJson(json, jsonString);
  client.publish(mqttTopic, jsonString.c_str());
  delay(1000); // Adjust delay time as needed
}


//------------------------------------------------------------------------------------------
// MQTT Connection Check function START
void ConnectionCheck(){
  if (!client.connected()) {
    reconnect();}}
// MQTT Connection Check function END
//------------------------------------------------------------------------------------------
// Read data function for PZEM-004t START
DynamicJsonDocument ReadData() {
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();
  bool alarm = pzem.getPowerAlarm();

  DynamicJsonDocument json(1024);
  json["DeviceId"] = "Device-1";
  JsonObject data = json.createNestedObject("data");
  data["Voltage"] = voltage;
  data["Current"] = current;
  data["Power"] = power;
  data["Energy"] = energy;
  data["Frequency"] = frequency;
  data["PF"] = pf;
  data["Alarm"] = alarm;
  return json;
}
// Read data function for PZEM-004t END
//------------------------------------------------------------------------------------------
// WiFi and MQTT COnnect function START
void Connect_WiFi_and_MQTT(){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    client.setServer(mqttServer, mqttPort);}
// WiFi and MQTT COnnect function END
//------------------------------------------------------------------------------------------
// MQTT Connect and Re-Connect Function
void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect(mqttClientID, mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);}}}
// MQTT Connect and Re-Connect Function END
//------------------------------------------------------------------------------------------