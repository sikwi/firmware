#include "SikwiWifiCloud.h"

SikwiWifiCloud::SikwiWifiCloud(SikwiWifi *wifi)
{
  this->wifi = wifi;
}

/******************************************************************************************/
/**************************************** Setup Server ************************************/
/******************************************************************************************/

bool SikwiWifiCloud::connect(const char *user, const char *password, void (* handlerFunction)(String, String)){
  this->tcpHandler = handlerFunction;

  this->wifi->writeCharPROGMEM(PSTR("SK+SIKWI=\""));
  this->wifi->writeCharPROGMEM(user);
  this->wifi->writeCharPROGMEM(PSTR("\","));
  this->wifi->writeCharPROGMEMln(password);

  return this->wifi->wait_for_esp_response(10000);
}

/*************************************************************************************/
/**************************************** Process ************************************/
/*************************************************************************************/

void SikwiWifiCloud::process()
{
  char *pb;
  if(this->wifi->newLine){
    if(strncmp_P(this->wifi->buffer, PSTR("SIKWI:"), 6)==0){
      uint8_t responseType;
      pb = this->wifi->buffer+6;
      if(tcpHandler){
        String name = strtok(pb, ":");
        String value = strtok(0, "");
        tcpHandler(name, value);
      }
    }//if found TCP:
  }
}
