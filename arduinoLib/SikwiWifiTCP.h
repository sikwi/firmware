//#define DEBUG_WIFI

#ifndef SikwiWifiTCP_h
#define SikwiWifiTCP_h

#include "SikwiWifi.h"
#include <avr/pgmspace.h>

class SikwiWifiTCP
{
    public:
      SikwiWifiTCP(SikwiWifi *wifi);

      void process();
      
      bool start(int port, void (* handlerFunction)(char*));
      bool send(String message);

    private:
      SikwiWifi *wifi;
      void (*tcpHandler)(char*);
};

#endif



