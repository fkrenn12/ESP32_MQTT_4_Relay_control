#include <wifi_mqtt.h>


// -----------------------------------------------------------------------------
WifiMQTT::WifiMQTT( /*MQTTClient* client ,
                    String      wifi_ssid, 
                    String      wifi_passwd, 
                    String      mqtt_hostname, 
                    uint32_t    mqtt_port,
                    String      mqtt_user,
                    String      mqtt_passwd,
                    bool        mqtt_use_tls */) // constructor 
// -----------------------------------------------------------------------------
{
    // ConnectingTask*
    // _client = client;
    _client = new MQTTClient(8096); // besser ausrechnen !!
    
    task = new ConnectingTask(  _client,
    /*
                                wifi_ssid,                          
                                wifi_passwd, 
                                mqtt_hostname, 
                                mqtt_port,
                                mqtt_user,
                                mqtt_passwd,
                                mqtt_use_tls,
                                */
                                &net_unsec,
                                &net_secure);
                                    
    xTaskCreatePinnedToCore(    ConnectingTask::run, 
                                "wifi_mqtt_connect", 
                                8092, 
                                (void*) task, 
                                1, // we need higher task priority than 0
                                &taskHandle,
                                APP_CPU_NUM); 
      
}; 
// -----------------------------------------------------------------------------
bool WifiMQTT::wifi_is_connected(void)
{
    return (task->wifi_is_connected);
}
// -----------------------------------------------------------------------------
bool WifiMQTT::mqtt_is_connected(void)
{
    return (task->mqtt_is_connected);
}
// -----------------------------------------------------------------------------
void WifiMQTT::onConnected(void (*funcptr)(void)) 
{
    task->onConnectptr = funcptr;
}
// -----------------------------------------------------------------------------
void WifiMQTT::onDisconnected(void (*funcptr)(void)) 
{
    task->onDisconnectptr = funcptr;
}
// -----------------------------------------------------------------------------
void WifiMQTT::onMessage(void (*funcptr)(String&,String&))
{
    _client->onMessage(funcptr);
}
// -----------------------------------------------------------------------------
void WifiMQTT::set_config(Config& config) 
{
    task->_wifi_ssid        = config.wifi_ssid;
    task->_wifi_passwd      = config.wifi_pass;
    task->_mqtt_hostname    = config.mqtt_hostname;
    task->_mqtt_passwd      = config.mqtt_password;
    task->_mqtt_port        = config.mqtt_port;
    task->_mqtt_user        = config.mqtt_user;
    task->_mqtt_passwd      = config.mqtt_password;
    task->_mqtt_use_tls     = config.auto_tls;
}
// -----------------------------------------------------------------------------
void WifiMQTT::start(void) 
{
   vTaskResume(taskHandle);
}
// -----------------------------------------------------------------------------
void WifiMQTT::stop(void) 
{
   vTaskSuspend(taskHandle);
}
// -----------------------------------------------------------------------------
void WifiMQTT::loop(void) 
{
   task->fire_loop = true;
}
// -----------------------------------------------------------------------------
bool WifiMQTT::subscribe(String& topic, int qos, unsigned int timeout_ms )
{
    unsigned int start_millis = millis();
    unsigned int timeout = 50;

    if (timeout_ms > 0)
    timeout = timeout_ms;


    while (true)
    {
        if (millis() - start_millis > timeout)
            return (false);

        if (millis() - task->last_loop_millis  > 50)
        {
            _client->subscribe(topic,qos);
            return (true);
        }
        vTaskDelay(1/portTICK_PERIOD_MS);
    } 
}
// -----------------------------------------------------------------------------
bool WifiMQTT::publish(String& topic, String& payload, bool retain, int qos, unsigned int timeout_ms) 
{
    
    unsigned int start_millis = millis();
    unsigned int timeout = 50;

    if (timeout_ms > 0)
        timeout = timeout_ms;

    
    while (true)
    {
        if (millis() - start_millis > timeout)
            return (false);

        if (millis() - task->last_loop_millis  > 50)
        {
            _client->publish(topic,payload,retain,qos);
            return (true);
        }
        vTaskDelay(1/portTICK_PERIOD_MS);
    }
}

// -----------------------------------------------------------------------------
bool WifiMQTT::publish(const char* topic, const char* payload, bool retain, int qos, unsigned int timeout_ms) 
{
    String _topic = topic;
    String _payload = payload;
    return (publish(_topic,_payload,retain,qos,timeout_ms));
}
