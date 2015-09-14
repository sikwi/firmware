unsigned int localPort = 2390;

const int NTP_PACKET_SIZE = 48;

#define NTP_ADDR 20

byte packetBuffer[ NTP_PACKET_SIZE];
WiFiUDP udp;

unsigned int ntpPort = 123;
uint8_t ntpIpPart1 = 0;
uint8_t ntpIpPart2 = 0;
uint8_t ntpIpPart3 = 0;
uint8_t ntpIpPart4 = 0;
//SK+NTP=128,138,141,172,123

unsigned long beginningTimeStamp = 0;

void initNTPServer(){
  udp.begin(localPort);
  
  ntpIpPart1 = EEPROM.read(NTP_ADDR);
  ntpIpPart2 = EEPROM.read(NTP_ADDR+1);
  ntpIpPart3 = EEPROM.read(NTP_ADDR+2);
  ntpIpPart4 = EEPROM.read(NTP_ADDR+3);
  ntpPort = EEPROM.read(NTP_ADDR+4);

  //If every Params are the same (so the eeprom is empty with 0 or 255)
  if(ntpIpPart1 == ntpIpPart2 && ntpIpPart3 == ntpIpPart2 && ntpIpPart4 == ntpIpPart3 && ntpPort == ntpIpPart3)
  {
    //set Default value
    ntpPort = 123;
    ntpIpPart1 = 128;
    ntpIpPart2 = 138;
    ntpIpPart3 = 141;
    ntpIpPart4 = 172;
  }
}

void setNTPServer(uint8_t ipPart1, uint8_t ipPart2, uint8_t ipPart3, uint8_t ipPart4, int port){
  EEPROM.write(NTP_ADDR, ipPart1);
  EEPROM.write(NTP_ADDR+1, ipPart2);
  EEPROM.write(NTP_ADDR+2, ipPart3);
  EEPROM.write(NTP_ADDR+3, ipPart4);
  EEPROM.write(NTP_ADDR+4, port);
  EEPROM.commit();
  
  ntpPort = port;
  ntpIpPart1 = ipPart1;
  ntpIpPart2 = ipPart2;
  ntpIpPart3 = ipPart3;
  ntpIpPart4 = ipPart4;

  updateTime();
}

void updateTime(){
  
  IPAddress timeServer(ntpIpPart1, ntpIpPart2, ntpIpPart3, ntpIpPart4);
  int cb = 0;
  unsigned long t1=millis();
  do{
    sendNTPpacket(timeServer);
  
    unsigned long t=millis();
    do{
      cb = udp.parsePacket();
    }
    while(!cb && millis()<t+1000);
  }
  while(!cb && millis()<t1+3000);
  
  if(cb>=1){
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    beginningTimeStamp = secsSince1900 - seventyYears - millis()/1000;
  }
}

unsigned long getCurrentTimeStamp(){
  if(beginningTimeStamp == 0)
    updateTime();
  return beginningTimeStamp + millis()/1000;
}

String getCurrentTime(){
  unsigned long currentTimeStamp = getCurrentTimeStamp();
  String time = String((currentTimeStamp  % 86400L) / 3600);
  time += ":";
  if ( ((currentTimeStamp % 3600) / 60) < 10 ) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    time += '0';
  }
  time += String((currentTimeStamp  % 3600) / 60); // print the minute (3600 equals secs per minute)
  time += ":";
  if ( (currentTimeStamp % 60) < 10 ) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    time += '0';
  }
  time += String((currentTimeStamp % 60));
  return time;
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, ntpPort); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
