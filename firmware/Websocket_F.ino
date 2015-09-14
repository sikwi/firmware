#define MAX_SRV_WS_CLIENTS 10
WiFiClient serverWSClients[MAX_SRV_WS_CLIENTS];
WebSocketServer webSocketServer;

void createServerWebsocket(int port) {
  if (serverWebsocket != NULL)
  {
    free(serverWebsocket);
  }

  serverWebsocket = new WiFiServer(port);
  aJson.addItemToObject(jsonConfigRoot, "ws", aJson.createItem(port));
  
  serverWebsocket->setNoDelay(true);
  serverWebsocket->begin();

  MDNS.addService("websocket", "ws", port);
}

void handleClientWebsocket() {
  uint8_t i;
  //check if there are any new clients
  if (serverWebsocket->hasClient()) {
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      //find free/disconnected spot
      if (!serverWSClients[i] || !serverWSClients[i].connected()) {
        if (serverWSClients[i]) serverWSClients[i].stop();
        serverWSClients[i] = serverWebsocket->available();
        
        unsigned long t=millis();
        while (!serverWSClients[i].available() && millis()<t+2000)
        {
          //Serial.println("wait");
        }
        webSocketServer.handshake(serverWSClients[i]);
        //Serial.print("New WS client: "); Serial.println(i);
        break;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = serverWebsocket->available();
    serverClient.stop();
  }
  //check clients for data
  for (i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (serverWSClients[i] && serverWSClients[i].connected()) {
      if (serverWSClients[i].available()) {
        //get data from the telnet client and push it to the UART
        unsigned long t=millis();
        while (serverWSClients[i].available() && millis()<t+2000){
          String data = webSocketServer.getData(serverWSClients[i]);
          if (data.length() > 0) {
            Serial.print("WS:");
            Serial.println(data);
            //webSocketServer.sendData("WS:"+data, serverWSClients[i]);
          }
        }
      }
    }
  }
}

void sendToWebsocket(String content) {
  for (uint8_t i = 0; i < MAX_SRV_WS_CLIENTS; i++) {
    if (serverWSClients[i] && serverWSClients[i].connected()) {
      webSocketServer.sendData(content, serverWSClients[i]);
      delay(1);
    }
  }
}

