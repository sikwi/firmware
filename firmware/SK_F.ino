int serialFindLength = 0;

void processSerialData() {
  if (Serial.available()) {
    char myChar = (char)Serial.read();
    
    if (myChar == '\n') {
      handleInputString();
      inputString = "";
      inputStringLenght = 0;
    } else if (myChar != '\r'){
      inputString += myChar;
      inputStringLenght++;
    }
  }
}

inline boolean serialFind(String string){
  serialFindLength = (string.length());
  return inputString.indexOf(string) == 0;
}

void handleInputString()
{
  int pastMillis;
  if(debug){pastMillis = micros();}
  
  if(echoMode)
  {
    Serial.println(inputString);
    Serial.println();
  }
                                                    //-------------------- SK
  if (serialFind("SK") && inputStringLenght == 2) { //-------------------- SK
    Serial.println("OK");
  }

                                                    //-------------------- SK+RST
  else if (serialFind("SK+RST")){                   //-------------------- SK+RST
    ESP.restart();
  }

  else if(serialFind("SK+DEBUG")){
    debug = serialFind("SK+DEBUG1");
    if(debug)
      Serial.println("Enable Debug Mode");
  }
                               //----------------------------------------- SKE1
  else if (serialFind("SKE")) {//----------------------------------------- SKE1
    Serial.println("OK");
    echoMode = serialFind("SKE1");
  }

  else if (serialFind("SK+ERASE")) {//----------------------------------------- SK+ERASE
    for(int i=0; i<4096; i++)
      EEPROM.write(i, 0);
    EEPROM.commit();
    Serial.println("OK");
  }


  else if (serialFind("SK+IP?")) {//------------------------------------- SK+IP? --> IP:1.2.3.4
    Serial.print("IP:");
    Serial.println(WiFi.localIP());
    Serial.println("OK");
    
  }

  else if (serialFind("SK+BAUD=")){//----------------------------------- SK+BAUD=9600
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      int baud = _result[0].toInt();

      if(baud==9600 || baud==19200 || baud==38400 || baud==57600 || baud==115200)
      {
        Serial.println("OK");
        delay(100);
        
        Serial.begin(baud);
        eeprom_write_int(BaudRateADDRESS, baud);
      }
      else{
        Serial.println("ERROR");
      }
    }else{
      Serial.println("ERROR");
    }
  }

  //--------------------------------------------------
  //------------------- REQUEST --------------------------
  //--------------------------------------------------
  
                                   //------------------------------------- SK+GET=URL
  else if (serialFind("SK+GET=")) {//------------------------------------- SK+GET=http://httpbin.org/ip
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      String url = _result[0];
      http_get(url.c_str(),"");
    }else
    {
      Serial.println("ERROR");
    }
  }
  
                                       //--------------------------------- SK+GETAUTH="name:pass","URL"
  else if (serialFind("SK+GETAUTH=")) {//--------------------------------- SK+GETAUTH="test@gmail.com:1234","http://httpbin.org/headers"
    int numberNeeded = 2;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      String auth = _result[0];
      String url = _result[1];
      
      http_getAuth(url.c_str(),auth);
    }else
    {
      Serial.println("ERROR");
    }
  }
                                    //----------------------------------- SK+POST="URL","VARIABLES"
  else if (serialFind("SK+POST=")) {//----------------------------------- SK+POST="http://httpbin.org/post","s=3"
    int numberNeeded = 2;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      Serial.println("OK");
      String url = _result[0];
      String variables = _result[1];
      http_post(url.c_str(),variables.c_str(),"", false);
    }else
    {
      Serial.println("ERROR");
    }
  }
                                        //----------------------------------- SK+POSTAUTH="AUTH","URL","VARIABLES"
  else if (serialFind("SK+POSTAUTH=")) {//----------------------------------- SK+POSTAUTH="test@gmail.com:1234","http://httpbin.org/headers","s=3"
    int numberNeeded = 3;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      Serial.println("OK");
      String auth = _result[0];
      String url = _result[1];
      String variables = _result[2];
      http_postAuth(auth, url.c_str(), variables.c_str());
    }else{
      Serial.println("ERROR");
    }
  }

  //--------------------------------------------------
  //------------------- SIKWI ------------------------
  //--------------------------------------------------

  else if (serialFind("SK+SIKWI=")){//----------------------------------- SK+MQTT_C=login,password
    int numberNeeded = 2;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      String user = _result[0];
      String pass = _result[1];
      connectToSikwi(user, pass);
    }else{
      Serial.println("ERROR");
    }
  }
  /*
  else if (serialFind("SK+SIKWI_G=")){//----------------------------------- SK+SIKWI_G=
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      String name = _result[0];
      sikwiGet(name);
    }else{
      Serial.println("ERROR");
    }
  }
  
  else if (serialFind("SK+SIKWI_P=")){//----------------------------------- SK+SIKWI_P=
    int numberNeeded = 2;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      sikwiPush(_result[0], _result[1], true);
    }else{
      Serial.println("ERROR");
    }
  }
  */
  //--------------------------------------------------
  //------------------- MQTT --------------------------
  //--------------------------------------------------
  
                                     //----------------------------------- SK+MQTT_C="SERVER_URL","SERVER_PORT"
  else if (serialFind("SK+MQTT_C=")){//----------------------------------- SK+MQTT_C="test.mosquitto.org",1883
    int numberNeeded = 3;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      String server = _result[0];
      int port = _result[1].toInt();
      String clientID = _result[2];
      connectToMQTT(server, port, clientID);
    }else{
      Serial.println("ERROR");
    }
  }
                                     //----------------------------------- SK+MQTT_S=TOPIC
  else if (serialFind("SK+MQTT_S=")){//----------------------------------- SK+MQTT_S="sikwi"
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      Serial.println("OK");
      String topic = _result[0];
      subscribeToTopic(topic);
    }else{
      Serial.println("ERROR");
    }
  }
                                     //----------------------------------- SK+MQTT_P="TOPIC","VALUE"
  else if (serialFind("SK+MQTT_P=")){//----------------------------------- SK+MQTT_P="sikwi",20
    int numberNeeded = 2;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded)){
      Serial.println("OK");
      int index;
      String topic = _result[0];
      String value = _result[1];
      publishToTopic(topic, value);
    }else{
      Serial.println("ERROR");
    }
  }
                                     //----------------------------------- SK+MQTT_U="USER","PASSWORD"
  else if (serialFind("SK+MQTT_U=")){//----------------------------------- SK+MQTT_U="test","1234"
    int numberNeeded = 2;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      String user = _result[0];
      String pwd = _result[1];
      setPassword(user, pwd);
      Serial.println("OK");
    }else{
      Serial.println("ERROR");
    } 
  }


  //--------------------------------------------------
  //------------------- HTTP --------------------------
  //--------------------------------------------------

                                    //----------------------------------- SK+HTTP="PORT"
  else if (serialFind("SK+HTTP=")) {//----------------------------------- SK+HTTP="80"
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      int port = _result[0].toInt();
      createServerHttp(port);
      Serial.println("OK");
    }else{
      Serial.println("ERROR");
    }
  }
  
                                     //----------------------------------- SK+HTTP_P="URL","NAME"
  else if (serialFind("SK+HTTP_P=")){//----------------------------------- SK+HTTP_P="http://cdn.sikwi.com/V1/","relay/"
    int numberNeeded = 2;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      homePageServer = _result[0];
      homePageName = _result[1];
      Serial.println("OK");
    }else{
      Serial.println("ERROR");
    }
  }

  else if (serialFind("SK+HTTP_D=")) {//----------------------------------- SK+DELAY=80
     int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      delayHandlerSerial = _result[0].toInt();
      Serial.println("OK");
    }else{
      Serial.println("ERROR");
    }
  }
                                  //------------------------------------ SK+RES
  else if (serialFind("SK+RES")) {//------------------------------------ SK+RES="<p><b>Hello</b>World</p>"
    char response = inputString[6];
    switch(response)
    {
      case 1:
      case '1':
      {
        mustSendHomePage=true;
        break;
      }
      case 2:
      case '2':
      {
        mustSendOnlyData=true;
        break;
      }
      case 3:
      case '3':
      {
        mustSend404=true;
        break;
      }
      case 4:
      case '4':
      {
        mustSendDefault=true;
        break;
      }
      case '=': //SK+RES=<p><b>coucou</b> ça va?</p>
      {
        int numberNeeded = 1;
        String _result[numberNeeded];
        if(splitCommand(inputString.substring(serialFindLength+1), _result, numberNeeded))
        {
          mustSendHTTP=_result[0];
        }else{
          mustSend404=true;
        }
        break;
      }
    }
    Serial.println("OK");
  }
  

  //--------------------------------------------------
  //------------------- DATA --------------------------
  //--------------------------------------------------
  
  else if (serialFind("SK+CONF?")){//---------------------------------- SK+DATA="meteo","calme"
    Serial.println(getConfJsonData());
    Serial.println("OK");
  }
                                   //---------------------------------- SK+DATA="Name","value"
  else if (serialFind("SK+DATA=")){//---------------------------------- SK+DATA="meteo","calme"
    int numberNeeded = 2;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      if(_result[0]=="name")
      {
        changeValueStringFor(_result[0], _result[1]);
        name = _result[1];
        aJson.addItemToObject(jsonConfigRoot, "name", aJson.createItem(name.c_str()));
      }
      else if(_result[1] == "true" || _result[1] == "false"){
        changeValueBoolFor(_result[0], _result[1]=="true");
      }
      else{
        char firstChar = inputString[serialFindLength+_result[0].length()+1];
        if (firstChar == '-' || (firstChar >= '0' && firstChar <= '9')){
          if(strchr(_result[1].c_str(), '.') != NULL || strchr(_result[1].c_str(), 'e') != NULL || strchr(_result[1].c_str(), 'E') != NULL)
            changeValueFloatFor(_result[0], _result[1].toFloat());
          else
            changeValueIntFor(_result[0], _result[1].toInt());
        }
        else{
          changeValueStringFor(_result[0], _result[1]);
        } 
      }
      Serial.println("OK");
    }else
    {
      Serial.println("ERROR");
    }
  }
  else if (serialFind("SK+DATA_D=")){//---------------------------------- SK+DATA_D="dimmer" //delete an object
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      deleteValue(_result[0]);
      Serial.println("OK");
    }else
    {
      Serial.println("ERROR");
    }
  }
  else if (serialFind("SK+DATA_H=")){//---------------------------------- SK+DATA_H="dimmer" //delete an object
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      if(setHistoryForValue(_result[0]))
        Serial.println("OK");
      else 
        Serial.println("ERROR");
    }else
    {
      Serial.println("ERROR");
    }
  }
  else if (serialFind("SK+DATA_M=")){//---------------------------------- SK+DATA_M=10 //delete an object
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      HISTORY_MAX_VALUE = _result[0].toInt();
      Serial.println("OK");
    }else
    {
      Serial.println("ERROR");
    }
  }
  else if (serialFind("SK+DATA?=")){//---------------------------------- SK+DATA_N="dimmer","100.2"
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      Serial.println(getValueFor(_result[0]));
      Serial.println("OK");
    }else
    {
      Serial.println("ERROR");
    }
  }
  else if (serialFind("SK+DATA?")){//---------------------------------- SK+DATA_N="dimmer","100.2"
    Serial.println(jsonDataString);
    Serial.println("OK");
  }
  
  //--------------------------------------------------
  //------------------- TCP --------------------------
  //--------------------------------------------------

                                    //----------------------------------- SK+TCP="port"
  else if (serialFind("SK+TCP=")) {//----------------------------------- SK+TCP=23
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      Serial.println("OK");
      int port = _result[0].toInt();
      createServerTelnet(port);
    }else
    {
      Serial.println("ERROR");
    }
  }
                                 //----------------------------------- TCP:"message"
  else if (serialFind("TCP>")) {//----------------------------------- TCP:"coucou"
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      Serial.println("OK");
      String data = _result[0];
      sendToTelnet(data);
    }else
    {
      Serial.println("ERROR");
    }
  }

  
  //--------------------------------------------------
  //------------------- NTP AND TIME -----------------
  //--------------------------------------------------
  
  else if (serialFind("SK+NTP=")) {//----------------------------------- SK+TCP=23
    int numberNeeded = 5;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      setNTPServer(_result[0].toInt(),_result[1].toInt(), _result[2].toInt(), _result[3].toInt(),_result[4].toInt());
      
      Serial.println(getCurrentTime());
      Serial.println("OK");
    }else
    {
      Serial.println("ERROR");
    }
  }
  
  else if (serialFind("SK+TIME?")) {//----------------------------------- SK+404
    Serial.println(getCurrentTime());
    Serial.println("OK");
  }

  else if (serialFind("SK+TIME_S?")) {//----------------------------------- SK+404
    Serial.println(getCurrentTimeStamp());
    Serial.println("OK");
  }
  
  //--------------------------------------------------
  //------------------- WEBSOCKET --------------------
  //--------------------------------------------------

                                    //----------------------------------- SK+WS="port"
  else if (serialFind("SK+WS=")) {//----------------------------------- SK+WS=8081
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      Serial.println("OK");
      int port = _result[0].toInt();
      createServerWebsocket(port);
    }else
    {
      Serial.println("ERROR");
    }
  }
                                 //----------------------------------- WS:"message"
  else if (serialFind("WS>")) {//----------------------------------- WS:"coucou"
    int numberNeeded = 1;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      Serial.println("OK");
      String data = _result[0];
      sendToWebsocket(data);
    }else
    {
      Serial.println("ERROR");
    }
  }
 
  
  //--------------------------------------------------
  //------------------- WIFI -------------------------
  //--------------------------------------------------

  else if (serialFind("SK+WIFI?")) {//----------------------------------- SK+WJAP?
    Serial.print("SSID:");
    Serial.println(WiFi.SSID());
    Serial.print("IP:");
    Serial.println(WiFi.localIP());
    Serial.print("St:");
    Serial.println(WiFi.status());
    Serial.println("OK");
  }
                                    //----------------------------------- SK+WCJAP="SSID"
  else if (serialFind("SK+WCJAP")) {//----------------------------------- SK+WCJAP="MyWifi"
    char response = inputString[8];
    String wifiName = "";
    if(response == '='){
      int numberNeeded = 1;
      String _result[numberNeeded];
      if(splitCommand(inputString.substring(serialFindLength+1), _result, numberNeeded))
      {
        wifiName = _result[0];
      }  
    }

    if(wifiName == "")
      wifiName = "SIKWI_"+name;

    if(createAPModeConnexion(wifiName)){
      Serial.print("IP:");
      Serial.println(WiFi.localIP());
    }else{
      Serial.print("SSID:");
      Serial.println(wifiName);
    }
    Serial.println("OK");
  }
                                   //----------------------------------- SK+WCAP="SSID","PASS","automatic"
  else if (serialFind("SK+WCAP=")) {//----------------------------------- SK+WCAP="MyWifi","1234",true
    if(debug){Serial.println("createAP0");delay(100);}
    int numberNeeded = 3;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      if(debug){Serial.println("createAP01");delay(100);}
      String ssid = _result[0];
      String password = _result[1];
      bool automatic = _result[2]=="true";
      if(debug){Serial.println("createAP02");delay(100);}
      
      if(createAP(ssid, password, automatic)){
        if(debug){Serial.println("createAP03");delay(100);}
        
        Serial.print("IP:");
        Serial.println(WiFi.localIP());
        Serial.println("OK");
      }
      else{
        Serial.println("ERROR"); 
      }
    }else
    {
      Serial.println("ERROR");
    }
  }
  
                                    //----------------------------------- SK+WJAP="SSID","PASS"
  else if (serialFind("SK+WJAP=")) {//----------------------------------- SK+WJAP="MyWifi","1234"
    int numberNeeded = 2;
    String _result[numberNeeded];
    if(splitCommand(inputString.substring(serialFindLength), _result, numberNeeded))
    {
      String ssid = _result[0];
      String password = _result[1];
      if(connectToWifi(ssid, password)){
        Serial.print("IP:");
        Serial.println(WiFi.localIP());
        Serial.println("OK");
      }
      else{
        Serial.println("ERROR"); 
      }
    }else
    {
      Serial.println("ERROR");
    }
  }

  else{
    Serial.println("UNKWN");
  }

  if(debug){Serial.print("parse in ");Serial.print(micros()-pastMillis);delay(100);}
}

boolean splitCommand(String command, String *fillArray, int number)
{
  int index = 0, index2 = 0, i;
  
  String comparator;
  for (i = 0; i<number; i++){      
    if(command[index] == '\"')
    {
      index++;
      
      if(i == number-1)
        comparator = "\"";
      else
        comparator = "\",";
        
      index2 = command.indexOf(comparator, index);
      if(index2 == -1)
        return false;
        
    }else{
      if(i == number-1){
        index2 = command.length();
      }else{
        comparator = ",";
        
        index2 = command.indexOf(comparator, index);
        if(index2 == -1)
          return false;
      }
    }
    
    if(i == number-1){
      if(index2 != command.length()-1 && index2 != command.length())
        return false;
    }
    
    fillArray[i] = command.substring(index, index2);
    index = index2+comparator.length();
  }

  return true;
}
