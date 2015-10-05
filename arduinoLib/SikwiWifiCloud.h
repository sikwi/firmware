//#define DEBUG_WIFI

#ifndef SikwiWifiCloud_h
#define SikwiWifiCloud_h

#include "SikwiWifi.h"
#include <avr/pgmspace.h>

class SikwiWifiCloud
{
    public:
      SikwiWifiCloud(SikwiWifi *wifi);

      void process();

      bool connect(const char *user, const char *password, void (* handlerFunction)(String, String));

    private:
      SikwiWifi *wifi;
      void (*tcpHandler)(String, String);
};

#endif
