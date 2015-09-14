//#define DEBUG_WIFI

#ifndef SikwiWifi_h
#define SikwiWifi_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>

#define BUFFER_SIZE 100

class SikwiWifi
{
    public:
      SikwiWifi();

      bool reset();
      bool reset(int baudRate, Print *debugPrinter);

      bool resetFirmwareDefault();

      bool process();

      bool connectToWifi(const char *ssid, const char *pass);
      bool setBaudRate(int baudRate);

      String getTime();
      String getTimeStamp();
      bool setNTPServer(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, int port);

      void writeCharPROGMEM(const char *signMessage);
      void writeCharPROGMEMln(const char *signMessage);

      bool wait_for_esp_response(const unsigned long timeout);
      bool wait_for_esp_response(const unsigned long timeout, const char* term);

      bool setName(String value);

      bool setData(const char * name, String value);
      bool setData(const char * name, const char* value);
      bool setData(const char * name, int value);
      bool setData(const char * name, double value);
      bool setData(const char * name, float value);
      bool setData(const char * name, bool value);

      String getData(const char * name);
      String getAllData();

      bool deleteData(const char * name);
      bool enableHistoryFor(const char * name);
      bool setMaxHistoryItem(uint8_t number);

      SoftwareSerial *espSerial;
      int enablePin = A3;
      int debugPin = A0;

      unsigned long previousMillis = 0;
      Print *debugPrinter;

      char buffer[BUFFER_SIZE];
      bool newLine = false;

      void printVirgule();

      bool read_till_eol();

    private:
      const long intervalTest = 10000;

      void sendDataHeader(const char* name);
};

// Derive from ADS1105 & override construction to set properties
class SikwiWifiMini : public SikwiWifi
{
 public:
  SikwiWifiMini();

 private:
};

// Derive from ADS1105 & override construction to set properties
class SikwiWifiOne : public SikwiWifi
{
 public:
  SikwiWifiOne();

 private:
};

#endif
