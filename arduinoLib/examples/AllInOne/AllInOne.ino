#include <SoftwareSerial.h>
#include <SikwiWifi.h>
#include <SikwiWifiHttp.h>
#include <SikwiWifiTCP.h>
#include <SikwiWifiWebsocket.h>
#include <SikwiWifiMQTT.h>

#define WIFI_SSID "XXXX"
#define WIFI_PASS "XXXX"

SikwiWifiOne wifi;
SikwiWifiHttp http(&wifi);
SikwiWifiTCP tcp(&wifi);
SikwiWifiWebsocket ws(&wifi);
SikwiWifiMQTT mqtt(&wifi);

#define SIKWI_SERVER_URL "//cdn.sikwi.com/V1/"
#define SIKWI_PAGE_NAME "dimmer/"
#define SIKWI_NAME "testDimmer"


uint8_t ledStatus = 0;

void resetWifi(){
  wifi.reset(&Serial);
  
  if(!wifi.connectToWifi(PSTR(WIFI_SSID), PSTR(WIFI_PASS)))
    return resetWifi();  
  
  
  static SikwiWifiHandler serverHandlers[] = {
    { METHOD_GET, PSTR("/*")},
    { METHOD_POST, PSTR("/*"), httpLed }
  };
  http.start(80, serverHandlers, (sizeof(serverHandlers) / sizeof(serverHandlers[0])), PSTR(SIKWI_NAME));
  http.setHomePageParams(PSTR(SIKWI_SERVER_URL), PSTR(SIKWI_PAGE_NAME));

  tcp.start(23, tcpHandler);
  ws.start(8081, wsHandler);

  mqtt.setUser(PSTR("client1"),PSTR("1234"));
  mqtt.start(PSTR("mqtt.sikwi.com"),1883, PSTR(SIKWI_NAME));
  mqtt.subscribe(PSTR("client1/#"), mqttHandler);
  
  updateObject();

  Serial.println("Init");
}

void setup()
{
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
  tcp.process();
  ws.process();
  mqtt.process();
}

uint8_t httpLed(uint8_t method, char* url, char* variables)
{
  ledStatus = http.getVariableIntIn(F("s"), variables);
  Serial.print("url:");
  Serial.println(url);
  Serial.print("variables:");
  Serial.println(variables);
  
  analogWrite(3, ledStatus*2.55);
  analogWrite(5, ledStatus*2.55);
  analogWrite(6, ledStatus*2.55);
  updateObject();

  mqtt.publish(PSTR("client1/dimmer/"), ledStatus);

  return RESPONSE_DATA;
}

void tcpHandler(char* message){
  Serial.print("messageTCP:");
  Serial.println(message);
  
  mqtt.publish(PSTR("client1/#"), message);
}

void wsHandler(char* message){
  Serial.print("messageWS:");
  Serial.println(message);
}

void mqttHandler(char* topic, char* value){
  Serial.print("messageMQTT:");
  Serial.print(topic);
  Serial.print(">>");
  Serial.println(value);
}

void updateObject()
{
  String data = F("{\"s\":");
  data += ledStatus;
  data += F("}");
  
  wifi.setData(data);
}
