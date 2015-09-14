#define MAX_SRV_CLIENTS 10
WiFiClient serverClients[MAX_SRV_CLIENTS];

void createServerTelnet(int port){
  if(serverTelnet != NULL)
  {
    free(serverTelnet);
  }
  
  serverTelnet = new WiFiServer(port);
  aJson.addItemToObject(jsonConfigRoot, "tcp", aJson.createItem(port));
  
  serverTelnet->setNoDelay(true);
  serverTelnet->begin();

  MDNS.addService("tcp", "tcp", port);
}

void handleClientTelnet(){
  uint8_t i;
  //check if there are any new clients
  if (serverTelnet->hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = serverTelnet->available();
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = serverTelnet->available();
    serverClient.stop();
  }
  //check clients for data
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        //get data from the telnet client and push it to the UART
        String res = "";
        
        unsigned long t=millis();
        while (serverClients[i].available() && millis()<t+2000){
          res += (char)serverClients[i].read();
        }
        Serial.print("TCP:");
        Serial.println(res);
      }
    }
  }
}

void sendToTelnet(String content){
  for(int i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      serverClients[i].write(content.c_str(), content.length());
      serverClients[i].write("\n", 1);
      delay(1);
    }
  }
}

