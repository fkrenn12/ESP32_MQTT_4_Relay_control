#ifndef _RELAY_H
#define _RELAY_H

#include <Arduino.h>
#include <Wemos_d1_r32.h>

// definition of eeprom address
#define eeprom_addr_relay_state   0

#define relay1                  arduino_pin7 
#define relay2                  arduino_pin6
#define relay3                  arduino_pin5
#define relay4                  arduino_pin4

void handle_message(String &topic, String &payload);

#endif
