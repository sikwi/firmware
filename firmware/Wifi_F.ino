#define SSID_ADDR 30

boolean connectWifiFromEeprom(){
  String ssid = "";
  String password = "";
  
  for (int i = 0; i < 32; ++i) {
    ssid += char(EEPROM.read(SSID_ADDR+i));
  }
  
  for (int i = 32; i < 96; ++i) {
    password += char(EEPROM.read(SSID_ADDR+i));
  }
  
  if(debug){
    Serial.print(F("connectWifiFromEeprom:"));
    Serial.print(ssid); 
    Serial.print(F(", password:"));
    Serial.println(password);
    delay(100);
  }
  
  return connectToWifi(ssid, password);
}

boolean saveWifiToEeprom(String ssid, String password){
  for (int i = 0; i < 96; ++i) {
    EEPROM.write(SSID_ADDR+i, 0);
  }
  for (int i = 0; i < ssid.length(); ++i) {
    EEPROM.write(SSID_ADDR+i, ssid[i]);
  }
  for (int i = 0; i < password.length(); ++i) {
    EEPROM.write(SSID_ADDR + 32 + i, password[i]);
  }
  EEPROM.commit();  
}

boolean connectToWifi(String ssid, String password){
  if(ssid == "")
    return false;
  
  if(ssid == WiFi.SSID() && WiFi.status() == WL_CONNECTED)
  {
    //Serial.print("already connected to this wifi:");
    //Serial.print(ssid);
    //Serial.print(",");
    //Serial.println(password);
    return true;
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  unsigned long t1 = millis();
  unsigned long t;
  unsigned long timeout = 20000;
  unsigned long timeout2 = 10000;
  if(debug) Serial.print("begin : " + t1);
  
  while (millis() < t1 + timeout){
    WiFi.begin(ssid.c_str(), password.c_str());
    if(debug) Serial.println("WiFi.begin");
            
    t = millis();
    uint8_t state;
    
    while (millis() < t + timeout2) {
      state = WiFi.status();
      
      if(debug){
        if(state==WL_DISCONNECTED && state != WL_IDLE_STATUS)
        {
          Serial.print(".");
        }
        else
        {
          Serial.print(state);
        }
      }
      
      if(state != WL_DISCONNECTED)
      {
        if(state == WL_CONNECTED){
          if(debug){
            Serial.println("connected");
            Serial.println();
          }
          return true;
        }else if(state == WL_IDLE_STATUS){
          return false;
        }
        break;
      }
      
      delay(100);
    }

    if(debug) Serial.println();
  }
  return false;
}

boolean createAP(String ssid, String password, bool automaticConnexion){ 
  
  if(debug){Serial.println("createAP1");delay(100);}

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  WiFi.mode(WIFI_AP);
  if(debug){Serial.println("createAP2");delay(100);}
  
  WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
  if(debug){Serial.println("createAP3");delay(100);}

  WiFi.softAP(ssid.c_str(), password.c_str());
  if(debug){Serial.println("createAP4");delay(100);}

  createServerHttp(80);
  if(debug){Serial.println("createAP6");delay(100);}
  

  if(automaticConnexion){
    if(debug){Serial.println("automatic connexion");delay(100);}
    
    if(dnsServer == NULL)
      dnsServer = new DNSServer();
    dnsServer->start(53, "*", AP_IP);
  }

  if(debug){Serial.println("createAP7");delay(100);}
  
  
  return true;
}

boolean createAPModeConnexion(String WifiName){
  currentWifiName = WifiName;
  
  if(connectWifiFromEeprom())
  {
    if(debug){
      Serial.println(F("connected from eeprom"));
      delay(100);
    }
    
    return true;
  }

  resetApModeConnexion();
  return false;
}

void resetApModeConnexion(){
  APModeConnexion = true;
  
  if(debug){
    Serial.print(F("cant connect to wifi, create an Access point SSID : "));
    Serial.println(currentWifiName+"_XXX");
    delay(100);
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  delay(100);
  for (int i = 0; i < n; ++i) {
    SSID_LIST += F("<option value=\"");
    SSID_LIST += WiFi.SSID(i);
    SSID_LIST += F("\">");
    SSID_LIST += WiFi.SSID(i);
    SSID_LIST += F("</option>");
  }
  delay(100);
 
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
  randomSeed(ESP.getCycleCount());
  WiFi.softAP(String(currentWifiName+"_"+random(9999)).c_str());

  createServerHttp(80);
  
  if(dnsServer == NULL)
    dnsServer = new DNSServer();
  dnsServer->start(53, "*", AP_IP);
}

/*
typedef enum {
        WL_NO_SHIELD = 255,
        WL_IDLE_STATUS = 0,
        WL_NO_SSID_AVAIL = 1,
        WL_SCAN_COMPLETED = 2,
        WL_CONNECTED = 3,
        WL_CONNECT_FAILED = 4,
        WL_CONNECTION_LOST = 5,
        WL_DISCONNECTED = 6
} wl_status_t;
*/
