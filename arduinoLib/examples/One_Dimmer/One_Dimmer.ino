#include <SoftwareSerial.h>
#include <SikwiWifi.h>
#include <SikwiWifiHttp.h>
#include <SikwiWifiWebsocket.h>

#define WIFI_SSID "XXXX"
#define WIFI_PASS "XXXX"

SikwiWifiOne wifi;

SikwiWifiHttp http(&wifi);
SikwiWifiWebsocket ws(&wifi);

#define SIKWI_SERVER_URL "//cdn.sikwi.com/V1/"
#define SIKWI_PAGE_NAME "dimmer/"
#define SIKWI_NAME "Leds Cuisine"

uint8_t ledStatus = 0;

void resetWifi(){
  Serial.println("resetWifi");
  delay(100);
  wifi.reset(9600, &Serial);
  Serial.println("wifi reseted");
  delay(100);
  
  if(!wifi.connectToWifi(PSTR(WIFI_SSID), PSTR(WIFI_PASS)))
  {
      Serial.println("Wifi do not exist or can't be joined");
      return resetWifi();
  }
    
  static SikwiWifiHandler serverHandlers[] = {
    { METHOD_GET, PSTR("/*")},
    { METHOD_POST, PSTR("/*"), httpLed }
  };
  http.start(80, serverHandlers, (sizeof(serverHandlers) / sizeof(serverHandlers[0])), PSTR(SIKWI_NAME));
  http.setHomePageParams(PSTR(SIKWI_SERVER_URL), PSTR(SIKWI_PAGE_NAME));
  
  ws.start(8081, wsHandler);
  
  Serial.println("Init");

  String test = wifi.getDataForName(PSTR("name"));
  Serial.print("test:");
  Serial.println(test); 
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
  ws.process();
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

void wsHandler(char* message){
  //Serial.print("messageWS:");
  //Serial.println(message);
}
