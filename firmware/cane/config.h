
#define SSID "UwU"
#define SSID_PASS "12345678"


#define DEVICE_ID "hello"


#define V_TO_TRIGGER 2.75

//Board pin
#define SDA 8
#define SCL 9
#define LED 41
#define BUZZER  45
#define LDR 15


//MQTT Conf
#define MQTT_BROKER
#define MQTT_USER 
#define MQTT_PASS 
#define TOPIC_PREFIX



//MQTT Topic
#define MQTT_TOPIC_SEND_LOCATE TOPIC_PREFIX "/" DEVICE_ID "/location"
#define MQTT_TOPIC_SEND_FALL TOPIC_PREFIX "/" DEVICE_ID "/fall"
#define MQTT_TOPIC_SEND_SL TOPIC_PREFIX "/" DEVICE_ID "/soundandlight"

#define MQTT_TOPIC_RECIEVE_ACTIONS  TOPIC_PREFIX "/" DEVICE_ID "/action"

