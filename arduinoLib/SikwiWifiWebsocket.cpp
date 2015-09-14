#include "SikwiWifiWebsocket.h"

SikwiWifiWebsocket::SikwiWifiWebsocket(SikwiWifi *wifi)
{
  this->wifi = wifi;
}

/******************************************************************************************/
/**************************************** Setup Server ************************************/
/******************************************************************************************/

bool SikwiWifiWebsocket::start(int port, void (* handlerFunction)(char*))
{
  this->tcpHandler = handlerFunction;
  
  this->wifi->writeCharPROGMEM(PSTR("SK+WS="));
  this->wifi->espSerial->println(port);
  return this->wifi->wait_for_esp_response(1000);
}

bool SikwiWifiWebsocket::send(String message)
{  
  this->wifi->writeCharPROGMEM(PSTR("WS>"));
  this->wifi->espSerial->println(message);
  return this->wifi->wait_for_esp_response(1000);
}

/*************************************************************************************/
/**************************************** Process ************************************/
/*************************************************************************************/

void SikwiWifiWebsocket::process()
{
  char *pb;
  if(this->wifi->newLine){
    if(strncmp_P(this->wifi->buffer, PSTR("WS:"), 3)==0){
      uint8_t responseType;
      pb = this->wifi->buffer+3;
      if(tcpHandler)
        tcpHandler(pb);
    }//if found TCP:
  }
}