#include <SoftwareSerial.h>
#include <SikwiWifi.h>
#include <SikwiWifiHttp.h>
#include <SikwiWifiAccessPointConfiguration.h>

SikwiWifiOne wifi;
SikwiWifiAccessPointConfiguration accessPoint(&wifi);
SikwiWifiHttp http(&wifi);

#define SIKWI_SERVER_URL "//cdn.sikwi.com/V1/"
#define SIKWI_PAGE_NAME "default/"
#define SIKWI_NAME "Leds Cuisine"

uint8_t ledStatus = 0;

void resetWifi(){
  wifi.reset(9600, &Serial);
  
  wifi.setData(PSTR("name"), SIKWI_NAME); //SetName
  
  if(!accessPoint.connect())
    return resetWifi();//Error while creating AccessPoint, or join new wifi,...
    
  static SikwiWifiHandler serverHandlers[] = {
    { METHOD_GET, PSTR("/*")},
    { METHOD_POST, PSTR("/*"), httpLed }
  };
  http.start(80, serverHandlers, (sizeof(serverHandlers) / sizeof(serverHandlers[0])));
  http.setHomePageParams(PSTR(SIKWI_SERVER_URL), PSTR(SIKWI_PAGE_NAME));
}

void setup()
{
  pinMode(A0, OUTPUT);
  
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  
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
 
  http.process();
}

uint8_t httpLed(uint8_t method, char* url, char* variables)
{
  ledStatus = http.getVariableIntIn(F("s"), variables);
  Serial.print("url:");
  Serial.println(url);
  Serial.print("variables:");
  Serial.println(variables);

  digitalWrite(A0, ledStatus);

  analogWrite(3, ledStatus*2.55);
  analogWrite(5, ledStatus*2.55);
  analogWrite(6, ledStatus*2.55);
  
  wifi.setData(PSTR("s"), ledStatus);

  return RESPONSE_DATA;
}

