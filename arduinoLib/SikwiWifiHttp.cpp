#include "SikwiWifiHttp.h"

SikwiWifiHttp::SikwiWifiHttp(SikwiWifi *wifi)
{
  this->wifi = wifi;
}

/******************************************************************************************/
/**************************************** Setup Server ************************************/
/******************************************************************************************/

bool SikwiWifiHttp::setHomePageParams(const char *url, const char *pageName){
  this->wifi->writeCharPROGMEM(PSTR("SK+HTTP_P=\""));
  this->wifi->writeCharPROGMEM(url);
  this->wifi->writeCharPROGMEM(PSTR("\","));
  this->wifi->writeCharPROGMEMln(pageName);
  return this->wifi->wait_for_esp_response(1000);
}

bool SikwiWifiHttp::start(int port, SikwiWifiHandler *httpServerHandlers, uint8_t numOfHandlers)
{
  this->httpServerHandlers = httpServerHandlers;
  this->httpServerHandlersLength = numOfHandlers;

  this->wifi->writeCharPROGMEM(PSTR("SK+HTTP="));
  this->wifi->espSerial->println(port);
  return this->wifi->wait_for_esp_response(1000);
}

/*************************************************************************************/
/**************************************** Process ************************************/
/*************************************************************************************/

void SikwiWifiHttp::process()
{
  uint8_t method;
  char *pb;
  bool allowAll = false;

  if(this->wifi->newLine){
    if(strncmp_P(this->wifi->buffer, PSTR("HTTP:"), 5)==0){
      uint8_t responseType;
      method = this->wifi->buffer[5] - '0';
      pb = this->wifi->buffer+7;

      for(uint8_t x = 0;x < this->httpServerHandlersLength; x++){
        uint8_t length = strlen_P(this->httpServerHandlers[x].requestMatches);
        if(pgm_read_byte_near(this->httpServerHandlers[x].requestMatches + length-1)=='*'){
          allowAll=true;
          length--;
        }

        if(method == this->httpServerHandlers[x].method && strncmp_P(pb, this->httpServerHandlers[x].requestMatches,length)==0){
          char* pbg = pb+strlen_P(this->httpServerHandlers[x].requestMatches);
          if(pbg[0]==',' || allowAll){
            if(this->httpServerHandlers[x].handlerFunction){
              char* url = strtok(pb, ",");
              pb = strtok(0, "");
              
              responseType = (this->httpServerHandlers[x].handlerFunction)(method, url, pb);
              break;
            }else{
              responseType = RESPONSE_DEFAULT;
              break;
            }
          } 
        }//if match*/
      }//for httpHandler

      this->wifi->writeCharPROGMEM(PSTR("SK+RES"));
      this->wifi->espSerial->write(responseType);
      this->wifi->espSerial->println();
      this->wifi->wait_for_esp_response(1000);
    }//if found CC:
  }
}

/********************************************************************************************/
/**************************************** Utils ****************************************/
/********************************************************************************************/

char* SikwiWifiHttp::getVariableStringIn(const __FlashStringHelper *name,const char* source)
{
  char* command = strdup(source);
  command = strtok(command, "&");
  while (command != 0)
  {
      char* separator = strchr(command, '=');
      if (separator != 0)
      {
          *separator = 0;
          ++separator;
          if(strncmp_P(command, (const char *)name, strlen_P((const char *)name))==0)
          {
            free(command);
            return separator;
          }
      }
      command = strtok(0, "&");
  }

  free(command);
  return "";
}

int SikwiWifiHttp::getVariableIntIn(const __FlashStringHelper *name,const char* source)
{
  return atoi(this->getVariableStringIn(name, source));
}

