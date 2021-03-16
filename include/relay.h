#ifndef _RELAY_H
#define _RELAY_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Wemos_d1_r32.h>
#include <wifi_mqtt.h>

#define MINIMUM_RELAIS_SWITCH_INTERVAL_MS 100
// definition of eeprom address
#define eeprom_addr_relay_state   0

#define relay1      arduino_pin7 
#define relay2      arduino_pin6
#define relay3      arduino_pin5
#define relay4      arduino_pin4

class Relay4
{
   private:
        bool        next_relais_1_status;
        bool        next_relais_2_status;
        bool        next_relais_3_status;
        bool        next_relais_4_status;
        uint64_t    relais_1_last_switch_time;
        uint64_t    relais_2_last_switch_time;
        uint64_t    relais_3_last_switch_time;
        uint64_t    relais_4_last_switch_time;
   public:
        Relay4(void); // constructor
        void handle_message(WifiMQTT* mqtt, int accessnumber, String &topic, String &payload);
        void begin(int);
};
#endif
