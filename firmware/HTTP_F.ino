void createServerHttp(int port){ 
  if(debug){Serial.println("createServerHttp");delay(100);}
  
  String nameMDNS = name;
  nameMDNS.replace(" ","");
  if(debug){Serial.println("createServerHttp1");delay(100);}
  
  int sizeName = nameMDNS.length()+1;
  char __name[sizeName];
  nameMDNS.toCharArray(__name, sizeName);
  if(debug){Serial.println("createServerHttp2");delay(100);}

  if(wifi_get_opmode() == WIFI_AP)
  {
      if(debug){
        Serial.print("MDNS begin with:");
        Serial.print(nameMDNS);
        Serial.print(", ip:");
        Serial.println(AP_IP);
        delay(100);
      }
      mdns.begin(__name, AP_IP);
  }else{
      if(debug){
        Serial.print("MDNS begin with:");
        Serial.print(nameMDNS);
        Serial.print(", ip:");
        Serial.println(WiFi.localIP());
        delay(100);
      }
      mdns.begin(__name, WiFi.localIP());
  }

  if(debug) Serial.println("create Server");
  delay(100);
  
  if(serverHTTP != NULL)
  {
    free(serverHTTP);
  }
  
  serverHTTP = new ESP8266WebServer(port);
  serverHTTP->onNotFound(handlePage);  
  serverHTTP->begin();
  
  MDNS.addService("http", "tcp", port);
}

void handlePage() {
  digitalWrite(led, 1);

  if(debug) Serial.println("user ask for :"+serverHTTP->uri()+", APModeConnexion:"+APModeConnexion);
  
  if(APModeConnexion)
  {
    if(String(serverHTTP->uri()).endsWith("/setap")){
      String ssid = urlDecode(serverHTTP->arg("ssid"));
      String pass = urlDecode(serverHTTP->arg("pass"));
      if(debug)
      {
        Serial.print("User defined :");
        Serial.print(ssid);
        Serial.print(", password:");
        Serial.println(pass);
      }
    
      saveWifiToEeprom(ssid, pass);
    
      if(connectWifiFromEeprom()){
        Serial.println("SK+RDYWC");
      }else{
        saveWifiToEeprom("", "");
        Serial.println("ERROR");
        //resetApModeConnexion();
      }
    }
    else{
      if(debug) Serial.println("handleAPModePage");
      handleAPModePage("");
    }
    digitalWrite(led,0);
  }
  else{
    mustSendHomePage = false;
    mustSend404 = false;
    mustSendOnlyData = false;
    mustSendDefault = false;
    mustSendHTTP = "";
    
   String serverURI = serverHTTP->uri();
    if(serverURI == "/favicon.ico")
    {
      serverHTTP->send(404, "text/plain", "not found : "+serverURI);
      return;
    }else if (serverURI == "/data")
    {
      mustSendOnlyData = true;
      serveHomePage(serverURI);
      
      digitalWrite(led, 0);
      return;
    }
    
    String smess = "HTTP:";
    smess += serverHTTP->method();
    smess += ",";
    smess += serverURI;
    smess += ",";
    for (uint8_t i = 0; i < serverHTTP->args(); i++) {
      if(i!=0)
         smess += "&";
      smess += serverHTTP->argName(i) + "=" + serverHTTP->arg(i);
      //if(serverHTTP->argName(i) == "plain" && serverHTTP->arg(i) == "")
      //  Serial.println("You need to use a newer version of the ESP8266WebServer library from https://github.com/me-no-dev/Arduino/blob/d4894b115e3bbe753a47b1645a55cab7c62d04e2/hardware/esp8266com/esp8266/libraries/ESP8266WebServer/src/Parsing.cpp");   
    }
    
    Serial.println(smess);
    
    unsigned long t = millis();
    while (millis() < t + delayHandlerSerial) {
      processSerialData();
      if (mustSendHomePage || mustSend404 || mustSendOnlyData || mustSendDefault || mustSendHTTP != "")
        break;
    }
  
    if(mustSendHTTP != ""){
      serverHTTP->send(200, "text/html", mustSendHTTP);
    }
    else{
      if(!mustSend404 && !mustSendHomePage && !mustSendOnlyData || mustSendDefault) //if nothing has been send by Serial
      {
        mustSend404 = (serverURI!="/");
        if(serverHTTP->method() == 1)
          mustSendHomePage = !mustSend404;
        else
          mustSendOnlyData = !mustSend404;
      }
      
      if (serverURI.indexOf("/data") == 0)
        mustSendOnlyData = true;
    
      ///---
      serveHomePage(serverURI);
      ///--- 
    }
    
    digitalWrite(led, 0);
    
    mustSendHomePage = false;
    mustSend404 = false;
    mustSendOnlyData = false;
    mustSendDefault = false;
    mustSendHTTP = ""; 
  }
}

void serveHomePage(String serverURI)
{
  if (mustSend404)
  {
    serverHTTP->send(404, "text/plain", "not found : "+serverURI);
  }
  else if (mustSendOnlyData)
  {
    serverHTTP->send(200, "text/plain", jsonDataString);
  }
  else if(mustSendHomePage)
  {
    String page = F("<html><head><link rel=icon href=\"");
    page += homePageServer;
    page += F("favicon.ico\"><link rel=stylesheet href=");
    page += homePageServer;
    page += F("i.css><meta name=viewport content='initial-scale=1.0'>");
    /*page += "<meta id=config data-c=\"";
    page += getConfJsonData();
    page += ">";*/
    page += F("<title>");
    page += name;
    page += F("</title></head><body><p>");
    page += jsonDataString;
    page += F("</p><iframe src=");
    page += homePageServer;
    page += homePageName;
    page += F("></iframe><script src=");
    page += homePageServer;
    page += F("j.js></script></body></html>");
    serverHTTP->send(200, "text/html", page);
  }
}

void handleAPModePage(String s){
  s += F("<p>Please select the SSID of the network you wish to connect to, enter thepassword and then submit.</p><form method=\"get\" action=\"setap\"><label>SSID: </label><select name=ssid>");
  s += SSID_LIST;
  s += F("</select><label>Password: </label><input name=pass length=64 type=password><input id=button type=submit></form>");
  serverHTTP->send(200, "text/html", makePage(s));
}

String makePage(String content){
  String s = F("<!DOCTYPE html><html><head><title>Sikwi</title><meta name=viewport content=\"width=device-width,user-scalable=0\"><style>html, body {width : 100%;height: 100%;font-family: sans-serif;margin: 0;}body *{-webkit-appearance: none;font-weight: 100;margin: 0;padding: 0;display: block;}header{height: 110px;background-color: #2196f3;color: #FFF;line-height: 150px;font-size: 30px;text-align: center;}h1{margin-top: 20px;}p{margin: 10px 0px;text-align: justify;}#content {max-width: 300px;margin: auto;color : #999;}select, input{border: 1px solid #ccc;font-size: 16px;color : #2196f3;font-weight: 400;}select{display: inline;height: 40px;padding: 5px;width: 100%;background: #F5F5F5;}input{height: 25px;width: 278px;padding: 5px 10px;}#button{width: 100%;height: 70px;background-color: #2196f3;color: #FFF;margin: 30px 0;}label{margin: 20px 0 5px;}</style></head><body><header>");
  s += name;
  s += F("</header><div id=content><h1>Wi-Fi Settings</h1>");
  s += content;
  s += F("</div></body></html>");
  return s;
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  //s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}

