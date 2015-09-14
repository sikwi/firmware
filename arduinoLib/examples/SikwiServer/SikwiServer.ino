#include <SoftwareSerial.h>
#include <SikwiWifi.h>
#include <SikwiWifiServer.h>
#include <SikwiWifiAccessPointConfiguration.h>

#define SIKWI_USER "sikwi"
#define SIKWI_PASS "test"

#define WIFI_SSID "XXXX"
#define WIFI_PASS "XXXX"

#define SIKWI_NAME "TestBreadboard"

SikwiWifiOne wifi;
SikwiWifiAccessPointConfiguration accessPoint(&wifi);
SikwiWifiServer server(&wifi);

uint8_t ledStatus = 0;
String inputSerialString;

void resetWifi(){
  wifi.reset(9600, &Serial);
  
  wifi.setName(SIKWI_NAME); //SetName

  if(!accessPoint.connect())
    return resetWifi();//Error while creating AccessPoint, or join new wifi,...
  
  server.connect(PSTR(SIKWI_USER), PSTR(SIKWI_PASS), serverDataHandler);

  updateObject();
}

void setup()
{
  pinMode(A0, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.begin(115200);  

  Serial.println("setup");
  delay(100);
  
  resetWifi();
}

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
  
  server.process();
}

void serverDataHandler(String type, String value)
{
  if(type == "switch")
  {
    ledStatus = value.toInt();
    
    Serial.print("type:");
    Serial.println(type);
    Serial.print("value:");
    Serial.println(value);
    
    digitalWrite(A0, ledStatus);
    digitalWrite(8, ledStatus);
    
    updateObject();
  }
}


void updateObject()
{
  wifi.setData(PSTR("switch"), ledStatus);
}
