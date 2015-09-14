//#define DEBUG_WIFI

#ifndef SikwiWifiServer_h
#define SikwiWifiServer_h

#include "SikwiWifi.h"
#include <avr/pgmspace.h>

class SikwiWifiServer
{
    public:
      SikwiWifiServer(SikwiWifi *wifi);

      void process();
      
      bool connect(const char *user, const char *password, void (* handlerFunction)(String, String));

    private:
      SikwiWifi *wifi;
      void (*tcpHandler)(String, String);
};

#endif



