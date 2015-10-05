#define FIRMWARE_VERSION "00_00_01"

#include <stdint.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <MQTT.h> //https://github.com/i-n-g-o/esp-mqtt-arduino/
#include <WebSocketServer.h>//https://github.com/Tibus/ESP8266-Websocket
#include <aJSON.h> //https://github.com/interactive-matter/aJson
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include <WiFiUdp.h>

#define BaudRateADDRESS 10

MQTT *myMqtt = NULL;
MQTT *sikwiServer = NULL;

MDNSResponder mdns;
ESP8266WebServer *serverHTTP;
WiFiServer *serverTelnet;
WiFiServer *serverWebsocket;
DNSServer *dnsServer;


String homePageServer = "http://cdn.sikwi.com/V1/";
String homePageName = "default/";
String inputString = "";
String deviceName = "???";
String uid = "???";

boolean debug = false;

int inputStringLenght = 0;
String SSID_LIST;
String currentWifiName;

const int led = 2;
const IPAddress AP_IP(192, 168, 1, 1);

boolean APModeConnexion = false;

boolean mustSendHomePage = false;
boolean mustSend404 = false;
boolean mustSendOnlyData = false;
boolean mustSendDefault = false;
String mustSendHTTP = "";
int delayHandlerSerial = 2000;

boolean echoMode = true;

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  uid = wifi_station_get_hostname();
  uid.replace("ESP_", "");

  wifi_station_set_hostname((char*)uid.c_str());

  EEPROM.begin(4096);

  int baud;
  eeprom_read_int(BaudRateADDRESS, &baud);

  if (baud != 9600 && baud != 19200 && baud != 38400 && baud != 57600 && baud != 115200)
    baud = 9600;

  Serial.begin(baud);
  eeprom_write_int(BaudRateADDRESS, baud);

  initData();
  initNTPServer();

  delay(3000);

  Serial.println();
  Serial.print("SK+VER:");
  Serial.println(FIRMWARE_VERSION);

  Serial.println();
  Serial.println("SK+RDY");
  //WiFi.printDiag(Serial);
  //Serial.println(WiFi.status());
}

void loop(void) {
  if (dnsServer != NULL)
    dnsServer->processNextRequest();

  if (serverHTTP != NULL)
    serverHTTP->handleClient();

  if (serverTelnet != NULL)
    handleClientTelnet();

  if (serverWebsocket != NULL)
    handleClientWebsocket();

  processSerialData();
}
