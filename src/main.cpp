/*
    Target: ESP32
*/
// -------------------------------------------------------------------------------------------
// #define CONFIG_FREERTOS_UNICORE
#include "main.h"
#define DEBUG                 0

#define NODE_ROOT             "maqlab"
#define NODE_MODELNAME        "NAME OF MODEL"
#define NODE_MANUFACTORER     "NAME OF MANUFACTORER"
#define NODE_DEVICETYPE       "TYPE OF DEVICE"
#define NODE_VERSION          "1.0.0"

#define FORCE_CONFIG_PORTAL   0
#define BAUDRATE              9600
   

#define led_wifi                arduino_pin2
#define led_mqtt                arduino_pin3
#define force_config_portal_pin arduino_pin12


// ------------> Time Server settings <------------------
#define NTP_SERVER           "de.pool.ntp.org"
#define TZ_INFO              "WEST-1DWEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00" // Western European Time
// -------------------------------------------------------------------------------------------
Config    config;
WifiMQTT  mqtt;
MQTTNode  node(&mqtt, NODE_ROOT , NODE_MANUFACTORER, NODE_MODELNAME, NODE_DEVICETYPE, NODE_VERSION );

// prototypes
void mqtt_message(String &topic, String &payload);
void mqtt_disconnected(void);
void mqtt_connected(void);

//-------------------------------------------------------------------
void mqtt_disconnected()
//-------------------------------------------------------------------
{
    Serial.println("*** DISONNECTED ***");
}
//-------------------------------------------------------------------
void mqtt_connected()
//-------------------------------------------------------------------
{
    Serial.println("*** CONNECTED *** ");
    node.subscribe();
}
//-------------------------------------------------------------------
void mqtt_message(String &topic, String &payload) 
//-------------------------------------------------------------------
{ 
  struct tm local;
  getLocalTime(&local);      
  String time = String(local.tm_hour) + ":" + String(local.tm_min) + ":" + String(local.tm_sec);
  Serial.println(time + " Received: " + topic + " " + payload);
  
  node.handle_standard_commands(topic, payload);
  if (node.is_message_for_this_device(topic)) 
  {
    /*
    Code to pass on 
    */ 
    Serial.println(" OHH, message for me!!");
  }
}

//--------------------------------------------
void setup() 
//--------------------------------------------
{   
    // pin configuration 
    pinMode(led_wifi, OUTPUT);
    pinMode(led_mqtt, OUTPUT);
    pinMode(force_config_portal_pin,INPUT_PULLUP);
    digitalWrite(led_wifi, 0);
    digitalWrite(led_mqtt, 0);
    // serial configuration
    Serial.begin(BAUDRATE,SERIAL_8N1);
    Serial.setDebugOutput(true);
    Serial.setDebugOutput(false);  // stop debug output on serial
    // welcome 
    Serial.println("\n*** STARTUP after RESET ***");
    Serial.println("CHIPID: " + config.chipid);
    Serial.println("CPU-Core # " + String(xTaskGetAffinity(NULL)));
    Serial.println("MAX HEAPSIZE: " + String(ESP.getMaxAllocHeap()));
    Serial.println("Devicename: " + node.get_devicefullname());
    Serial.println("Acessnumber: " + String(node.get_accessnumber()));

    // init filesystem and config file
    if (!config.initialize()) ESP.restart();  // without the file system nothing will work
    if (!config.exist())    config.create();  // create initial configuration file
    if (!config.exist())      ESP.restart();  // without config file nothing will work too
       
    // reading the input pin to force the wifi portal
    bool needConfigPortal= !(bool)digitalRead(force_config_portal_pin);
    while(!(bool)digitalRead(force_config_portal_pin))
    {
      Serial.println("Remove jumper!");
      delay(1000);
    };
  
    // start portal if pin was low and load configuration 
    if (needConfigPortal) 
    {
      config.portal(config,node.get_accessnumber());
      config.printout(config);
      config.store(config);
    }
    config.load(config);

    // MQTT Broker connection start
    mqtt.config(config);
    mqtt.onConnected(mqtt_connected);
    mqtt.onDisconnected(mqtt_disconnected);  
    mqtt.onMessage(mqtt_message);
    mqtt.start();
    
    // node configuration
    node.set_root(config.mqtt_root);
    node.set_commandlist("[]");   
}

//--------------------------------------------
void loop() 
//--------------------------------------------
{     
  static uint32_t old_millis = millis();
  
  if (millis() - old_millis > 1000)
  {
    old_millis = millis();
    { 
      String topic = NODE_ROOT;
      topic.concat("/1/1/cmd/test");
      String payload = "0123456789";  
      mqtt.publish(topic,payload,false,0,100);
    }
  }
}

