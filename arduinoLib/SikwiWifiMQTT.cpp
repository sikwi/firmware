#include "SikwiWifiMQTT.h"

SikwiWifiMQTT::SikwiWifiMQTT(SikwiWifi *wifi)
{
  this->wifi = wifi;
}

/******************************************************************************************/
/**************************************** Setup Server ************************************/
/******************************************************************************************/

bool SikwiWifiMQTT::setUser(const char *user, const char *password){
  this->wifi->writeCharPROGMEM(PSTR("SK+MQTT_U=\""));
  this->wifi->writeCharPROGMEM(user);
  this->wifi->writeCharPROGMEM(PSTR("\","));
  this->wifi->writeCharPROGMEMln(password);

  return this->wifi->wait_for_esp_response(10000);
}

bool SikwiWifiMQTT::start(const char *server, int port, const char *name)
{  
  this->wifi->writeCharPROGMEM(PSTR("SK+MQTT_C=\""));
  this->wifi->writeCharPROGMEM(server);
  this->wifi->writeCharPROGMEM(PSTR("\","));
  this->wifi->espSerial->print(port);
  this->wifi->writeCharPROGMEM(PSTR(","));
  this->wifi->writeCharPROGMEMln(name);

  return this->wifi->wait_for_esp_response(10000);
}

bool SikwiWifiMQTT::subscribe(const char *topic, void (* handlerFunction)(char*, char*))
{
  this->tcpHandler = handlerFunction;
  
  this->wifi->writeCharPROGMEM(PSTR("SK+MQTT_S=\""));
  this->wifi->writeCharPROGMEM(topic);
  this->wifi->writeCharPROGMEMln(PSTR("\""));
  return this->wifi->wait_for_esp_response(1000);
}

void SikwiWifiMQTT::publish(const char *topic)
{  
  this->wifi->writeCharPROGMEM(PSTR("SK+MQTT_P=\""));
  this->wifi->writeCharPROGMEM(topic);
  this->wifi->writeCharPROGMEM(PSTR("\","));
}

bool SikwiWifiMQTT::publish(const char *topic, String message)
{  
  this->publish(topic);
  this->wifi->espSerial->println(message);
  return this->wifi->wait_for_esp_response(1000);
}

bool SikwiWifiMQTT::publish(const char *topic, int message)
{  
  this->publish(topic);
  this->wifi->espSerial->println(message);
  return this->wifi->wait_for_esp_response(1000);
}

/*************************************************************************************/
/**************************************** Process ************************************/
/*************************************************************************************/

void SikwiWifiMQTT::process()
{
  char *pb;
  if(this->wifi->newLine){
    if(strncmp_P(this->wifi->buffer, PSTR("MQTT:"), 5)==0){
      uint8_t responseType;
      pb = this->wifi->buffer+5;
      if(tcpHandler){
        char* topic = strtok(pb, ":");
        pb = strtok(0, "");
        tcpHandler(topic, pb);
      }
    }//if found TCP:
  }
}