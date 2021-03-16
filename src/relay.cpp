#include <relay.h>

//-------------------
Relay4::Relay4(void)
//-------------------
{
    // configre relay pins
    pinMode(relay1, OUTPUT); 
    pinMode(relay2, OUTPUT);
    pinMode(relay3, OUTPUT);
    pinMode(relay4, OUTPUT);
    
    relais_1_last_switch_time = 0;
    relais_2_last_switch_time = 0;
    relais_3_last_switch_time = 0;
    relais_4_last_switch_time = 0;
}

//--------------------------------------------------------------------------------------------
void Relay4::begin(int count)
//--------------------------------------------------------------------------------------------
{
    EEPROM.begin(count);
    uint8_t eeprom_data = EEPROM.readChar(eeprom_addr_relay_state);
    digitalWrite(relay1,(uint8_t)(eeprom_data == 0xa1));
    eeprom_data = EEPROM.readChar(eeprom_addr_relay_state + 1);
    digitalWrite(relay2,(uint8_t)(eeprom_data == 0xa3));
    eeprom_data = EEPROM.readChar(eeprom_addr_relay_state + 2);
    digitalWrite(relay3,(uint8_t)(eeprom_data == 0xa5));
    eeprom_data = EEPROM.readChar(eeprom_addr_relay_state + 3);
    digitalWrite(relay4,(uint8_t)(eeprom_data == 0xa9));
    delay(10);
    next_relais_1_status = (bool)digitalRead(relay1);
    next_relais_2_status = (bool)digitalRead(relay2);
    next_relais_3_status = (bool)digitalRead(relay3);
    next_relais_4_status = (bool)digitalRead(relay4);
}

//--------------------------------------------------------------------------------------------
void Relay4::handle_message(WifiMQTT* mqtt, int accessnumber, String &topic, String &payload) 
//--------------------------------------------------------------------------------------------
{
  bool next_relais_1_status = digitalRead(relay1);
  bool next_relais_2_status = digitalRead(relay2);
  bool next_relais_3_status = digitalRead(relay3);
  bool next_relais_4_status = digitalRead(relay4);


  int   need_publish_status   = 0;
  bool  handled               = false;
  bool  need_commit           = false;

  // without "cmd" it is not a valid command
  if (topic.lastIndexOf("cmd") < 0 ) return;

  String topic_modified    = topic;
  topic_modified.replace("cmd","rep");

  // Serial.println("TOPIC: " + topic);
  // Serial.println("TOPIC_MODIFIED: " + topic_modified);

  if (topic.lastIndexOf("rel/0?") >= 0)
  {
    need_publish_status = 1;
    handled = true;
  }
  else if (topic.lastIndexOf("rel/1?") >= 0)
  {
    mqtt->client->publish(topic_modified,String(digitalRead(relay1)));
    return;
  }
  else if (topic.lastIndexOf("rel/2?") >= 0)
  {
    mqtt->client->publish(topic_modified,String(digitalRead(relay2)));
    return;
  }
  else if (topic.lastIndexOf("rel/3?") >= 0)
  {
    mqtt->client->publish(topic_modified,String(digitalRead(relay3)));
    return;
  }
  else if (topic.lastIndexOf("rel/4?") >= 0)
  {
    mqtt->client->publish(topic_modified,String(digitalRead(relay4)));
    return;
  }
  // hier gehts los
  else if (topic.lastIndexOf("rel/0") >= 0)
  {
     payload.toLowerCase();
    
    if ( payload.lastIndexOf("on") >= 0 || payload.lastIndexOf("1") >= 0 || payload.lastIndexOf("true") >= 0) 
    {
      next_relais_1_status = true;
      next_relais_2_status = true;
      next_relais_3_status = true;
      next_relais_4_status = true;
      handled = true;
    }
    if ( payload.lastIndexOf("off") >= 0 || payload.lastIndexOf("0") >= 0 || payload.lastIndexOf("false")>= 0 )   
    {
      next_relais_1_status = false;
      next_relais_2_status = false;
      next_relais_3_status = false;
      next_relais_4_status = false;
      handled = true;
    }

  } 
  else if (topic.lastIndexOf("rel/1") >= 0)
  {
    payload.toLowerCase();   
    
    if ( payload.lastIndexOf("on") >= 0 || payload.lastIndexOf("1") >= 0 || payload.lastIndexOf("true") >= 0)  
    {
        next_relais_1_status = true;
        handled = true;
    }
    if ( payload.lastIndexOf("off") >= 0 || payload.lastIndexOf("0") >= 0 || payload.lastIndexOf("false")>= 0 )   
    {
        next_relais_1_status = false;
        handled = true;
    }
   

    
  }
  else if (topic.lastIndexOf("rel/2") >= 0)
  {
    payload.toLowerCase();
    if ( payload.lastIndexOf("on") >= 0 || payload.lastIndexOf("1") >= 0 || payload.lastIndexOf("true") >= 0) 
    {
        next_relais_2_status = true;
        handled = true;
    }
    if ( payload.lastIndexOf("off") >= 0 || payload.lastIndexOf("0") >= 0 || payload.lastIndexOf("false") >= 0)    
    {
        next_relais_2_status = false;
        handled = true;
    }
    
  }
  else if (topic.lastIndexOf("rel/3") >= 0)
  {
    payload.toLowerCase();
    if ( payload.lastIndexOf("on") >= 0 || payload.lastIndexOf("1") >= 0 || payload.lastIndexOf("true") >= 0) 
    {
        next_relais_3_status = true;
        handled = true;
    }
    if ( payload.lastIndexOf("off") >= 0 || payload.lastIndexOf("0") >= 0 || payload.lastIndexOf("false") >= 0)    
    {
        next_relais_3_status = false;
        handled = true;
    }
  }
  else if (topic.lastIndexOf("rel/4") >= 0)
  {
    payload.toLowerCase();
    if ( payload.lastIndexOf("on") >= 0 || payload.lastIndexOf("1") >= 0 || payload.lastIndexOf("true") >= 0) 
    {
        next_relais_4_status = true;
        handled = true;
    }
    if ( payload.lastIndexOf("off") >= 0 || payload.lastIndexOf("0") >= 0 || payload.lastIndexOf("false")>= 0 )    
    {
        next_relais_4_status = false;
        handled = true;
    }
  }
  if (next_relais_1_status != digitalRead(relay1))
  {
    if (millis() - relais_1_last_switch_time > MINIMUM_RELAIS_SWITCH_INTERVAL_MS)
    {
      relais_1_last_switch_time = millis();
      digitalWrite(relay1,(uint8_t)next_relais_1_status);
      EEPROM.writeByte(0,0xa0 | uint8_t(next_relais_1_status));
      //Serial.println("Relais 1 " + String((uint8_t)next_relais_1_status));
      need_commit = true;
    }
  }
  if (next_relais_2_status != digitalRead(relay2))
  {
    if (millis() - relais_2_last_switch_time > MINIMUM_RELAIS_SWITCH_INTERVAL_MS)
    {
      relais_2_last_switch_time = millis();
      digitalWrite(relay2,(uint8_t)next_relais_2_status);
      EEPROM.writeByte(1,0xa2 | uint8_t(next_relais_2_status));
      //Serial.println("Relais 2 " + String((uint8_t)next_relais_2_status));
      need_commit = true;
    }
  }
  if (next_relais_3_status != digitalRead(relay3))
  {
    if (millis() - relais_3_last_switch_time > MINIMUM_RELAIS_SWITCH_INTERVAL_MS)
    {
      relais_3_last_switch_time = millis();
      digitalWrite(relay3,(uint8_t)next_relais_3_status);
      EEPROM.writeByte(2,0xa4 | uint8_t(next_relais_3_status));
      //Serial.println("Relais 3 " + String((uint8_t)next_relais_3_status));
      need_commit = true;
    }
  }
  if (next_relais_4_status != digitalRead(relay4))
  {
    if (millis() - relais_4_last_switch_time > MINIMUM_RELAIS_SWITCH_INTERVAL_MS)
    {
      relais_4_last_switch_time = millis();
      EEPROM.writeByte(3,0xa8 | uint8_t(next_relais_4_status));
      digitalWrite(relay4,(uint8_t)next_relais_4_status);
      //Serial.println("Relais 4 " + String((uint8_t)next_relais_4_status));
      need_commit = true;
    }
  }
  if (need_commit) EEPROM.commit();
  if (handled)
  {
      //Serial.println("HANDLED");
      Serial.println(topic);
      mqtt->client->publish(topic_modified,"ACCEPTED");
      need_publish_status = 1;
  }
  else
  {
      //Serial.println("NOT HANDLED");
      mqtt->client->publish(topic_modified,"ERROR");
      return;
  }

  if (need_publish_status || need_commit)
  {
    // String topic    = topic_root + "rep/" + String(accessnumber);
    
    if (topic_modified != "")
    {
      // we do not need the command itself anymore for replying the status
      // so after rep we cut the string and readd the accessnumber 
      String rep = "/rep/";
      int index       = topic_modified.lastIndexOf(rep) + rep.length();
      topic_modified  = topic_modified.substring(0,index) + String(accessnumber);
      
      String payload  = "{'status':{'rel':{'1':'{{REL1}}', '2':'{{REL2}}', '3':'{{REL3}}', '4':'{{REL4}}'}}}";
      payload.replace("{{REL1}}", String(digitalRead(relay1)));
      payload.replace("{{REL2}}", String(digitalRead(relay2)));
      payload.replace("{{REL3}}", String(digitalRead(relay3)));
      payload.replace("{{REL4}}", String(digitalRead(relay4)));
      mqtt->client->publish(topic_modified, payload); 
    }
  }
}