/*
    Target: ESP32
*/
// -------------------------------------------------------------------------------------------
// #define CONFIG_FREERTOS_UNICORE
#include "main.h"

#define MODELNAME             "D-RGB-8X5"
#define MANUFACTORER          "F.Krenn-HTL-ET"
#define DEVICETYPE            "Display-RGB-8x5"
#define VERSION               "1.0.0"

#define FORCE_CONFIG_PORTAL   0
#define DEBUG                 0
#define BAUDRATE              9600
#define TLS_PORT_RANGE_START  8000
#define TLS_PORT_RANGE_END    8999      

// LED Matrix configuration
// 40 Pixels / Pin#13 @ Wemos d1 R32 / 2812Shield
#define led_data_pin arduino_pin13
#define rows_count  8
#define cols_count  5
#define pixel_count rows_count * cols_count

// ------------> Time Server settings <------------------
#define NTP_SERVER "de.pool.ntp.org"
#define TZ_INFO "WEST-1DWEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00" // Western European Time
// -------------------------------------------------------------------------------------------
Config config;
Adafruit_NeoPixel leds(pixel_count, led_data_pin, NEO_GRB + NEO_KHZ800); // verified settings
LED_Matrix display(rows_count, cols_count, leds);
MQTTNode node("maqlab", MANUFACTORER, MODELNAME, DEVICETYPE, VERSION);

#define led_green arduino_pin2
#define led_red   arduino_pin3
#define digital_input arduino_pin12

// String deviceNameFull;
// String topic_root = mqtt_root + "/";  
boolean needReconnect = false;

// -----------------------------------------
// get a Unique ID ( we use the mac-address )
// -----------------------------------------
uint64_t chipid;
char chipid_str[13];    // 6 Bytes = 12 Chars + \0x00 = 13 Chars

WifiMQTT              mqtt;


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
    Serial.println("CONNECTED :-) ");
    String topic = "maqlab/#";
    mqtt.subscribe(topic,0,500);
    Serial.println("Subscribed ");
    //node.subscribe(mqtt);
}
//-------------------------------------------------------------------
void messageReceived(String &topic, String &payload) 
//-------------------------------------------------------------------
{ 
  struct tm local;
  getLocalTime(&local);      
  // Serial.println("RECEIVING");
  // Printing out received message on serial port
  String time = String(local.tm_hour) + ":" + String(local.tm_min) + ":" + String(local.tm_sec);
  Serial.println(time + " Received: " + topic + " " + payload);
  /*
  node.handle_mqtt_message(topic, payload, mqtt);
  if (node.is_message_for_this_device(topic)) 
    display.handle_mqtt_message(topic,payload,mqtt);
  */
}

//--------------------------------------------
void setup() 
//--------------------------------------------
{   
    Serial.begin(BAUDRATE,SERIAL_8N1);
    Serial.setDebugOutput(true);
    Serial.setDebugOutput(false);
    Serial.println("\n*** STARTUP after RESET ***");


    Serial.println("CHIPID: " + config.chipid);
    Serial.println("CPU-Core # " + String(xTaskGetAffinity(NULL)));
    chipid=ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).
    sprintf(chipid_str,"%04X%08X",(uint16_t)(chipid>>32),(uint32_t)chipid);
    Serial.println("MAX HEAPSIZE: " + String(ESP.getMaxAllocHeap()));
    if (!config.initialize()) ESP.restart();  // without file system nothing works
    if (!config.exist()) config.create();
    
    
    display.begin();
    bool needConfigPortal = false;
    pinMode(digital_input,INPUT_PULLUP);
    delay(10);
    needConfigPortal = !(bool)digitalRead(digital_input);
    while(!(bool)digitalRead(digital_input))
    {
      Serial.println("Remove Wire");
      delay(200);
    };
   
    Serial.println("Devicename: " + node.get_devicefullname());
    Serial.println("Acessnumber: " + String(node.get_accessnumber()));
    // configure led pins
    pinMode(led_green, OUTPUT);
    pinMode(led_red, OUTPUT);
    digitalWrite(led_red, 0);
    digitalWrite(led_green, 0);
 
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
    mqtt.set_config(config);
    mqtt.onMessage(mqtt_message);
    mqtt.start();
    

    // node.set_root(mqtt_root);
    // node.set_commandlist("[\"setpixel_rgb\",\"setpixel_hsv\"]");
    // node.subscribe(mqtt); 
    
}

//--------------------------------------------
void loop() 
//--------------------------------------------
{     
  static uint32_t old_millis = millis();
  
  if (millis() - old_millis > 200)
  {
    old_millis = millis();
    { 
      mqtt.publish("maqlab/ruby/test","ok",false,0,100);
      mqtt.publish("maqlab/ruby/test1","ok1",false,0,100);
      mqtt.publish("maqlab/ruby/test2","ok2",false,0,100);
    }
  }
}

