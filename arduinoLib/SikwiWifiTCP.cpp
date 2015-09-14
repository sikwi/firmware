#include "SikwiWifiTCP.h"

SikwiWifiTCP::SikwiWifiTCP(SikwiWifi *wifi)
{
  this->wifi = wifi;
}

/******************************************************************************************/
/**************************************** Setup Server ************************************/
/******************************************************************************************/

bool SikwiWifiTCP::start(int port, void (* handlerFunction)(char*))
{
  this->tcpHandler = handlerFunction;
  
  this->wifi->writeCharPROGMEM(PSTR("SK+TCP="));
  this->wifi->espSerial->println(port);
  return this->wifi->wait_for_esp_response(1000);
}

bool SikwiWifiTCP::send(String message)
{  
  this->wifi->writeCharPROGMEM(PSTR("TCP>"));
  this->wifi->espSerial->println(message);
  return this->wifi->wait_for_esp_response(1000);
}

/*************************************************************************************/
/**************************************** Process ************************************/
/*************************************************************************************/

void SikwiWifiTCP::process()
{
  char *pb;
 
  if(this->wifi->newLine){
    if(strncmp_P(this->wifi->buffer, PSTR("TCP:"), 4)==0){
      uint8_t responseType;
      pb = this->wifi->buffer+4;
      if(tcpHandler)
        tcpHandler(pb);
    }//if found TCP:
  }
}