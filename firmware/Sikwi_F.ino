String sikwi_user = "";
String sikwi_password = "";
String sikwi_topicPrefix = "";
boolean sikwi_Connected = false;
boolean waitForResponse = false;

void connectToSikwi(String user, String password){
  if(sikwiServer != NULL)
  {
    Serial.println("disconnect");
    sikwiServer->disconnect();
    free(sikwiServer);
  }
  
  sikwi_user = user;
  sikwi_password = password;
  sikwi_topicPrefix = "/"+sikwi_user+"/"+uid;
  
  // setup callbacks
  sikwiServer = new MQTT(uid.c_str(), "www.sikwi.com", 1883);
  //sikwiServer = new MQTT(uid.c_str(), "192.168.1.41", 1883);
  
  sikwiServer->onConnected(sikwiConnectedCb);
  sikwiServer->onDisconnected(sikwiDisconnectedCb);
  sikwiServer->onPublished(sikwiPublishedCb);
  sikwiServer->onData(sikwiDataCb);
 
  sikwiServer->setUserPwd(user.c_str(), password.c_str());
  sikwiServer->connect();
}

void sikwiPush(String topic, String value){
  sikwiPush(topic, value, true);
}

void sikwiPush(String topic, String value, bool verbose){
  if(sikwi_Connected == false)
  {
    if(verbose) Serial.println("SIKWI Not Connected");
    return; 
  }
  
  if(sikwiServer->publish(sikwi_topicPrefix+"/"+topic, value))
  {
    if(verbose) Serial.println("OK");
    waitForResponse = true;
  }
  else
  {
    if(verbose) Serial.println("ERROR");
  }
}

void sikwiConnectedCb() {
  sikwi_Connected = true;
  sikwiServer->subscribe(sikwi_topicPrefix+"/#");
  sikwiPush("name", name);
  if(debug) Serial.println("SIKWI>C");
}

void sikwiDisconnectedCb() {
  sikwi_Connected = false;
  if(debug) Serial.println("SIKWI>D");
  delay(500);
  //sikwiServer->connect();
  connectToSikwi(sikwi_user, sikwi_password);
}

void sikwiPublishedCb() {
  //Serial.println("OK");
}

void sikwiDataCb(String& topic, String& data) {
  if(waitForResponse)
  {
    //Serial.println("waitForResponse");
    waitForResponse = false;
    return;
  }
  
  Serial.print("SIKWI:");
  Serial.print(topic.substring(sikwi_topicPrefix.length()+1));
  Serial.print(":");
  Serial.println(data);
}
