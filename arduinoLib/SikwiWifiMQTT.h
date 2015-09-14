//#define DEBUG_WIFI

#ifndef SikwiWifiMQTT_h
#define SikwiWifiMQTT_h

#include "SikwiWifi.h"
#include <avr/pgmspace.h>

class SikwiWifiMQTT
{
    public:
      SikwiWifiMQTT(SikwiWifi *wifi);

      void process();
      
      bool setUser(const char *user, const char *password);

      bool start(const char *server, int port, const char *name);
      
      bool subscribe(const char *topic, void (* handlerFunction)(char*, char*));

      bool publish(const char *topic, String message);
      bool publish(const char *topic, int message);

    private:
      SikwiWifi *wifi;
      void (*tcpHandler)(char*, char*);
      void publish(const char *topic);
};

#endif



