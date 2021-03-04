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
#define TLS_PORT_RANGE_START  8000
#define TLS_PORT_RANGE_END    8999      

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

    if (!config.initialize()) ESP.restart();  // without the file system nothing will work
    if (!config.exist())    config.create();  // create initial configuration file
    if (!config.exist())      ESP.restart();  // without config file nothing will work too
       
    bool needConfigPortal= !(bool)digitalRead(force_config_portal_pin);
    while(!(bool)digitalRead(force_config_portal_pin))
    {
      Serial.println("Remove Wire");
      delay(100);
    };
  
    config.load(config);
    config.printout(config);

    if (needConfigPortal) 
    {
      config.portal(config,node.get_accessnumber());
      config.store(config);
    }
    config.load(config);
    config.printout(config);
    // MQTT Broker connection start
    mqtt.onConnected(mqtt_connected);
    mqtt.onDisconnected(mqtt_disconnected);  
    mqtt.config(config);
    mqtt.onMessage(mqtt_message);
    mqtt.start();
    
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
      mqtt.publish("maqlab/ruby/test","ok",false,0,100);
      mqtt.publish("maqlab/ruby/test1","ok1",false,0,100);
      mqtt.publish("maqlab/ruby/test2","ok2",false,0,100);
    }
  }
}

