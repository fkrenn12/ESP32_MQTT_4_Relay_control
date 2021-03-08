#include <relay.h>

void handle_message(String &topic, String &payload) 
//-------------------------------------------------------------------
{
  static uint64_t relais_1_last_switch_time = 0;
  static uint64_t relais_2_last_switch_time = 0;
  static uint64_t relais_3_last_switch_time = 0;
  static uint64_t relais_4_last_switch_time = 0;

  bool next_relais_1_status;
  bool next_relais_2_status;
  bool next_relais_3_status;
  bool next_relais_4_status;

  boolean need_commit = false;
  int need_publish_status = 0;
  // Printing out received data on serial port
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(payload);
  bool handled = false;
  if (topic.lastIndexOf("rel/0?") >= 0)
  {
    need_publish_status = 1;
    handled = true;
  }
  else if (topic.lastIndexOf("rel/1?") >= 0)
  {
    client.publish(topic,String(digitalRead(relay1)));
    return;
  }
  else if (topic.lastIndexOf("rel/2?") >= 0)
  {
    client.publish(topic,String(digitalRead(relay2)));
    return;
  }
  else if (topic.lastIndexOf("rel/3?") >= 0)
  {
    client.publish(topic,String(digitalRead(relay3)));
    return;
  }
  else if (topic.lastIndexOf("rel/4?") >= 0)
  {
    client.publish(topic,String(digitalRead(relay4)));
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
      client.publish(topic,"ACCEPTED");
      need_publish_status = 1;
  }
  else
  {
      //Serial.println("NOT HANDLED");
      client.publish(topic,"ERROR");
      return;
  }

  if (need_publish_status || need_commit)
  {
    need_publish_status = 0;
    // String topic    = topic_root + "rep/" + String(accessnumber);
    String topic    = topic_last_received;
    if (topic != "")
    {
      int index = topic.lastIndexOf("/rep/") + 5;
      topic = topic.substring(0,index) + String(accessnumber);
      // Serial.println("TEST: " + topic);
      String payload  = "{'status':{'rel':{'1':'{{REL1}}', '2':'{{REL2}}', '3':'{{REL3}}', '4':'{{REL4}}'}}}";
      payload.replace("{{REL1}}", String(digitalRead(relay1)));
      payload.replace("{{REL2}}", String(digitalRead(relay2)));
      payload.replace("{{REL3}}", String(digitalRead(relay3)));
      payload.replace("{{REL4}}", String(digitalRead(relay4)));
      client.publish(topic, payload); 
    }
  }
}