//#define DEBUG_WIFI

#ifndef SikwiWifiAccessPointConfiguration_h
#define SikwiWifiAccessPointConfiguration_h

#include "SikwiWifi.h"
#include <avr/pgmspace.h>

class SikwiWifiAccessPointConfiguration
{
    public:
      SikwiWifiAccessPointConfiguration(SikwiWifi *wifi);

	  bool connect();
      bool connect(const char *APName);
      bool process();

    private:
      bool waitForConnexion();
      bool connectedToWifi = false;
      
      SikwiWifi *wifi;
};

#endif
