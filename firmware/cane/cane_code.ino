#include <WiFi.h>
#include <config.h>
#include <ArduinoJson.h> 
#include <FallDetector.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqtt(espClient);
FallDetector gyro ;

bool led_status = false ;
bool buzzer_status = true ;
bool manual_override_light = false;

void connect_mqtt();



float readLDRValue(){
  int rawValue = analogRead(LDR) ;
  float v = (rawValue / 4095.0 ) * 3.3 ;
  return v ;
}


void lightTrigger(){
  if(led_status){
    digitalWrite(LED , LOW) ;
    led_status = not led_status ;
  }
  else{
    digitalWrite(LED , HIGH) ;
    led_status = not led_status ;
  }
}

void  buzzerTrigger(){
  if(buzzer_status){
    digitalWrite(BUZZER , LOW) ;
    buzzer_status = not buzzer_status ;
  }
  else{
    digitalWrite(BUZZER , HIGH) ;
    buzzer_status = not buzzer_status ;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  
  Serial.print("Message: ");
  Serial.println(messageTemp);

if (String(topic) == MQTT_TOPIC_RECIEVE_ACTIONS) {
    if (messageTemp == "LIGHT") {
      Serial.println("Light action recieved");
      lightTrigger();
      manual_override_light = true;
    }
    else if (messageTemp == "SOUND"){
      Serial.println("Sound action recieved");
      buzzerTrigger() ;
    }
  }
}


void publishSL(){
  if (!mqtt.connected()) {
    connect_mqtt();
  }
  StaticJsonDocument<256> doc; 
  doc["light"] = led_status ;
  doc["sound"] = not buzzer_status ;

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  if(mqtt.publish(MQTT_TOPIC_SEND_SL , jsonBuffer)){
    Serial.println("Send SL success");
  }
  else{
    Serial.println("Send SL failed");
  }
}

void publishFall(){
  if (!mqtt.connected()) {
    connect_mqtt();
  }
  StaticJsonDocument<256> doc; 
  doc["caneID"] = DEVICE_ID;

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  if(mqtt.publish(MQTT_TOPIC_SEND_FALL , jsonBuffer)){
    Serial.println("Send fall success");
  }
  else{
    Serial.println("Send fall failed");
  }
}

void fallHandler(){
  gyro.update() ;

  if(gyro.isFalling()){
    Serial.println("Falling...") ;
    
    if (buzzer_status) {
        buzzerTrigger();
    }
    
    publishFall() ;
    delay(1000) ;
  }
}


void connect_mqtt() {
  Serial.printf("Connecting to MQTT broker at %s...\n", MQTT_BROKER);
  
  //Random client
  String clientId = "ESP32-" + String(random(0xffff), HEX);
  
  if (!mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
    Serial.print("Failed to connect to MQTT broker, state: ");
    Serial.println(mqtt.state());
    delay(5000);
  } else {
    Serial.println("MQTT broker connected.");
    mqtt.subscribe(MQTT_TOPIC_RECIEVE_ACTIONS) ;
  }
}

void setup() {
  Serial.begin(9600);
  
  WiFi.begin(SSID , SSID_PASS);

  while(!gyro.begin(SDA, SCL)){
    delay(500) ;
    Serial.println("Trying to connect MPU6050");
  }
  Serial.println("MPU6050 connected.") ;


  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Trying to connect WiFi...");
  }
  Serial.println("WiFi Connected!");

  mqtt.setServer(MQTT_BROKER, 1883); 
  mqtt.setCallback(callback) ;

  //Board pin
  pinMode(LED , OUTPUT) ;
  pinMode(BUZZER , OUTPUT) ;
  digitalWrite(LED , LOW) ;
  digitalWrite(BUZZER , HIGH) ;
}

void loop() {
  if (!mqtt.connected()) {
    connect_mqtt();
  }
  mqtt.loop();


//ldr checker (always check)
  static unsigned long lastRound = 0;
  float ldrV = readLDRValue() ;

  if (ldrV >= V_TO_TRIGGER){
      if (!led_status && !manual_override_light) {
        digitalWrite(LED, HIGH);
        led_status = true;
      }
    lastRound = millis();
  }
  else if (millis() - lastRound > 15000) {
    lastRound = millis();
    
    if (led_status) {
        digitalWrite(LED, LOW);
        led_status = false;
    }

    manual_override_light = false;
  }

  //delay of sl publish
  static unsigned long lastSL = 0;
  if (millis() - lastSL > 1000) {
    publishSL() ;
    lastSL = millis();
  }

  fallHandler() ;
}