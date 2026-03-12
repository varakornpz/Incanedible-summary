#include <WiFi.h>
#include <PubSubClient.h>
#include <TinyGPS++.h>

const char* ssid = "FaemOS";
const char* password = "fame12345678";
const char* mqtt_server = "iot.cpe.ku.ac.th"; // หรือ Broker ที่คุณใช้งาน
const char* topic_gps = "b6810503897/hello/location";
// กำหนดขา RX, TX สำหรับ GPS (สลับกับขาของ GPS Module)
#define RXDI 17
#define TXDI 18

// สร้าง Object สำหรับ GPS และ Serial
TinyGPSPlus gps;
HardwareSerial GPSSerial(1);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200); // ความเร็วในการคุยกับคอมพิวเตอร์
  GPSSerial.begin(9600, SERIAL_8N1,RXDI,TXDI);   // ความเร็วในการคุยกับ GPS
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  Serial.println("GPS Module Testing...");
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("Incanedible1234","b6810503897","varakorn.kaew@ku.th")) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
   
  }
}      
void loop() {
  // รับข้อมูลจากโมดูล GPS
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  while (GPSSerial.available() > 0) {
    if (gps.encode(GPSSerial.read())) {
      sendGpsData() ;
    }
  }

  // ถ้าผ่านไป 5 วินาทีแล้วยังไม่มีข้อมูลเข้ามาเลย
 
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("Error: No GPS detected. Check wiring!");
    delay(2000);
  }
}

//void displayInfo() {
 // Serial.print("Location: ");
//  if (gps.location.isValid()) {
//    Serial.print(gps.location.lat(), 6); // ละติจูด
//    Serial.print(",");
//    Serial.print(gps.location.lng(), 6); // ลองจิจูด
//  } else {
//    Serial.print("INVALID (Waiting for signal...)");
//  }

//  Serial.print("Satellites: ");
 // Serial.println(gps.satellites.value());
 // Serial.print("  Date/Time: ");
//  if (gps.date.isValid()) {
//    Serial.print(gps.date.month());
//    Serial.print("/");
//    Serial.print(gps.date.day());
//    Serial.print("/");
//    Serial.print(gps.date.year());
//  }
 
//  Serial.println();
//}
void sendGpsData() {
  static unsigned long lastSend = 0;
  // ส่งข้อมูลทุกๆ 5 วินาที เพื่อไม่ให้หนัก Broker เกินไป
  if (millis() - lastSend > 5000) {
    lastSend = millis();

    // สร้างข้อมูลในรูปแบบ JSON
    String payload = "{";
    payload += "\"lat\":" + String(gps.location.lat(), 6) + ",";
    payload += "\"lng\":" + String(gps.location.lng(), 6) + ",";
    payload += "\"sat\":" + String(gps.satellites.value());
    payload += "}";

    Serial.println("Publishing: " + payload);
    client.publish(topic_gps, payload.c_str());
  }
}