//#define DEBUG_WIFI

#ifndef SikwiWifiHttp_h
#define SikwiWifiHttp_h

#include "SikwiWifi.h"
#include <avr/pgmspace.h>

#define METHOD_GET 1
#define METHOD_POST 2
#define METHOD_PUT 3
#define METHOD_PATCH 4
#define METHOD_DELETE 5

#define RESPONSE_HTML 1
#define RESPONSE_DATA 2
#define RESPONSE_404 3
#define RESPONSE_DEFAULT 4


struct SikwiWifiHandler
{
    uint8_t method;
    const char *requestMatches;
    uint8_t (* handlerFunction)(uint8_t, char*, char*);
};

class SikwiWifiHttp
{
    public:
      SikwiWifiHttp(SikwiWifi *wifi);

      void process();
      
      bool setHomePageParams(const char *url, const char *pageName);

      bool start(int port, SikwiWifiHandler *httpServerHandlers, uint8_t numOfHandlers);
      
      char* getVariableStringIn(const __FlashStringHelper *name,const char* source);
      int getVariableIntIn(const __FlashStringHelper *name,const char* source);

    private:
      SikwiWifi *wifi;
      SikwiWifiHandler *httpServerHandlers;
      uint8_t httpServerHandlersLength;
};

#endif



