#include <SoftwareSerial.h>
#include <SikwiWifi.h>
#include <SikwiWifiHttp.h>

#define WIFI_SSID "XXXX"
#define WIFI_PASS "XXXX"

SikwiWifiOne wifi;
SikwiWifiHttp http(&wifi);

#define SIKWI_SERVER_URL "//cdn.sikwi.com/V1/"
#define SIKWI_PAGE_NAME "dimmer/"
#define SIKWI_NAME "testDimmer"

uint8_t ledStatus = 0;

void resetWifi(){
  wifi.reset(9600, &Serial);
  
  wifi.setName(SIKWI_NAME); //SetName
  /*
  if(!wifi.connectToWifi(PSTR(WIFI_SSID), PSTR(WIFI_PASS)))
    return resetWifi();  
  */
  static SikwiWifiHandler serverHandlers[] = {
    { METHOD_GET, PSTR("/*")},
    { METHOD_POST, PSTR("/*"), httpLed }
  };
  http.start(80, serverHandlers, (sizeof(serverHandlers) / sizeof(serverHandlers[0])));
  http.setHomePageParams(PSTR(SIKWI_SERVER_URL), PSTR(SIKWI_PAGE_NAME));

  updateObject();

  Serial.println("Init");
}

void setup()
{
  pinMode(A0, OUTPUT);
  Serial.begin(115200);  

  resetWifi();
}

String inputSerialString;

void loop()
{
  
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputSerialString += inChar;
    if (inChar == '\n') {
      wifi.espSerial->print( inputSerialString );
      inputSerialString = "";
    }
  }
  
  if(!wifi.process()){
    Serial.println("mustReset");
    resetWifi();
  }
  else{
    
  }
  http.process();
}

uint8_t httpLed(uint8_t method, char* url, char* variables)
{
  ledStatus = http.getVariableIntIn(F("s"), variables);
  Serial.print("url:");
  Serial.println(url);
  Serial.print("variables:");
  Serial.println(variables);
  
  digitalWrite(A0, ledStatus*2.55);
  
  updateObject();

  return RESPONSE_DATA;
}


void updateObject()
{
  wifi.setData(PSTR("s"), ledStatus);
}
