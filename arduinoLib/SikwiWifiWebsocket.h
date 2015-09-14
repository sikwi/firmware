//#define DEBUG_WIFI

#ifndef SikwiWifiWebsocket_h
#define SikwiWifiWebsocket_h

#include "SikwiWifi.h"
#include <avr/pgmspace.h>

class SikwiWifiWebsocket
{
    public:
      SikwiWifiWebsocket(SikwiWifi *wifi);

      void process();
      
      bool start(int port, void (* handlerFunction)(char*));
      bool send(String message);

    private:
      SikwiWifi *wifi;
      void (*tcpHandler)(char*);
};

#endif



