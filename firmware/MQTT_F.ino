String mqtt_user = "";
String mqtt_pwd = "";

void connectToMQTT(String server, int port, String nameMDNS){
  if(myMqtt != NULL)
  {
    myMqtt->disconnect();
    free(myMqtt);
  }

  int sizeURL = server.length()+1;
  char __urlTest[sizeURL];
  server.toCharArray(__urlTest, sizeURL);

  int sizeNameTest = nameMDNS.length()+1;
  char __name[sizeNameTest];
  nameMDNS.toCharArray(__name, sizeNameTest);

  // setup callbacks
  myMqtt = new MQTT(__name, __urlTest, port);
  
  myMqtt->onConnected(myConnectedCb);
  myMqtt->onDisconnected(myDisconnectedCb);
  myMqtt->onPublished(myPublishedCb);
  myMqtt->onData(myDataCb);

  if(mqtt_user != "")
    myMqtt->setUserPwd(mqtt_user.c_str(), mqtt_pwd.c_str());
  myMqtt->connect();
}

void setPassword(String user, String pwd){
  mqtt_user = user;
  mqtt_pwd = pwd;
    
  if(myMqtt == NULL)
  {
    //Serial.println("MQTT Not Connected");
    Serial.println("OK");
  }else{
    myMqtt->setUserPwd(user.c_str(), pwd.c_str());
    Serial.println("OK"); 
  }
}

void publishToTopic(String topic, String value){
  if(myMqtt == NULL)
  {
    Serial.println("MQTT Not Connected");
    return; 
  }
    
  if(myMqtt->publish(topic, value))
    Serial.println("OK");
  else
    Serial.println("ERROR");
}

void subscribeToTopic(String topic){
  if(myMqtt == NULL)
  {
    Serial.println("MQTT Not Connected");
    return; 
  }
    
  if(myMqtt->subscribe(topic))
    Serial.println("OK");
  else
    Serial.println("ERROR");
}

void myConnectedCb() {
  Serial.println("MQTT:C");
}

void myDisconnectedCb() {
  Serial.println("MQTT:D");
  //delay(500);
  //myMqtt->connect();
}

void myPublishedCb() {
  //Serial.println("OK");
}

void myDataCb(String& topic, String& data) {
  Serial.print("MQTT:");
  Serial.print(topic);
  Serial.print(":");
  Serial.println(data);
}



