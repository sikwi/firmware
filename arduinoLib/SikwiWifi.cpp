#include "SikwiWifi.h"

SikwiWifi::SikwiWifi()
{
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
}

SikwiWifiMini::SikwiWifiMini()
{
  this->espSerial = new SoftwareSerial(9,10);
}

SikwiWifiOne::SikwiWifiOne()
{
  this->espSerial = new SoftwareSerial(10,9);
}

bool SikwiWifi::reset()
{
  digitalWrite(enablePin, LOW);
  delay(100);
  digitalWrite(enablePin, HIGH);
  delay(100);

  this->wait_for_esp_response(10000, PSTR("SK+RDY"));

  do{
      this->writeCharPROGMEMln(PSTR("SKE0"));
  }while(!this->wait_for_esp_response(1000));

  delay(3000); //-------------------------------------------- TO REMOVE

  return true;
}

bool SikwiWifi::reset(int baudRate, Print *debugPrinter)
{
  this->debugPrinter = debugPrinter;
  this->espSerial->begin(baudRate);

  return this->reset();
}

bool SikwiWifi::setBaudRate(int baudRate){
  this->writeCharPROGMEM(PSTR("SK+BAUD="));
  this->espSerial->println(baudRate);
  bool result = this->wait_for_esp_response(1000);
  if(result)
    this->espSerial->begin(baudRate);
  return result;
}

bool SikwiWifi::resetFirmwareDefault(){
  this->writeCharPROGMEMln(PSTR("SK+ERASE"));
  return this->wait_for_esp_response(1000);
}

/******************************************************************************************/
/**************************************** NTP **************************************/
/******************************************************************************************/


String SikwiWifi::getTime(){
  this->writeCharPROGMEMln(PSTR("SK+TIME?"));
  if(this->wait_for_esp_response(4000))
    return String(strtok(this->buffer, "\r"));
  return "";
}

String SikwiWifi::getTimeStamp(){
  this->writeCharPROGMEMln(PSTR("SK+TIME_S?"));
  if(this->wait_for_esp_response(4000))
    return String(strtok(this->buffer, "\r"));
  return "";
}

bool SikwiWifi::setNTPServer(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, int port){
  this->writeCharPROGMEM(PSTR("SK+NTP="));
  this->espSerial->print(ip1);
  this->writeCharPROGMEM(PSTR(","));
  this->espSerial->print(ip2);
  this->writeCharPROGMEM(PSTR(","));
  this->espSerial->print(ip3);
  this->writeCharPROGMEM(PSTR(","));
  this->espSerial->print(ip4);
  this->writeCharPROGMEM(PSTR(","));
  this->espSerial->println(port);

  return this->wait_for_esp_response(4000);
}

/******************************************************************************************/
/**************************************** Setup Wifi **************************************/
/******************************************************************************************/

bool SikwiWifi::connectToWifi(const char *ssid, const char *pass)
{
  this->writeCharPROGMEM(PSTR("SK+WJAP="));
  this->writeCharPROGMEM(ssid);
  this->writeCharPROGMEM(PSTR(","));
  this->writeCharPROGMEMln(pass);

  bool result = this->wait_for_esp_response(30000);
  if(result)
  {
    this->debugPrinter->println(this->buffer);
  }

  this->previousMillis = millis();

  return result;
}

/******************************************************************************************/
/**************************************** DATA ************************************/
/******************************************************************************************/

bool SikwiWifi::setName(String value){
  return this->setData(PSTR("name"), value);
}

void SikwiWifi::sendDataHeader(const char* name){
  this->writeCharPROGMEM(PSTR("SK+DATA="));
  this->writeCharPROGMEM(name);
  this->writeCharPROGMEM(PSTR(","));
}

bool SikwiWifi::setData(const char * name, const char* value){
  sendDataHeader(name);
  this->espSerial->println(value);
  return this->wait_for_esp_response(1000);
}

bool SikwiWifi::setData(const char * name, String value){
  sendDataHeader(name);
  this->espSerial->println(value);
  return this->wait_for_esp_response(1000);
}

bool SikwiWifi::setData(const char * name, int value){
  sendDataHeader(name);
  this->espSerial->println(value);
  return this->wait_for_esp_response(1000);
}

bool SikwiWifi::setData(const char * name, double value){
  sendDataHeader(name);
  this->espSerial->println(value);
  return this->wait_for_esp_response(1000);
}

bool SikwiWifi::setData(const char * name, float value){
  sendDataHeader(name);
  this->espSerial->println(value);
  return this->wait_for_esp_response(1000);
}

bool SikwiWifi::setData(const char * name, bool value){
  sendDataHeader(name);
  this->writeCharPROGMEMln((value?PSTR("true"):PSTR("false")));
  return this->wait_for_esp_response(1000);
}

String SikwiWifi::getData(const char * name){
  this->writeCharPROGMEM(PSTR("SK+DATA?="));
  this->writeCharPROGMEMln(name);
  if(this->wait_for_esp_response(1000))
    return String(strtok(this->buffer, "\r"));
  return "";
}

String SikwiWifi::getAllData(){
  this->writeCharPROGMEMln(PSTR("SK+DATA?"));
  if(this->wait_for_esp_response(1000))
    return String(strtok(this->buffer, "\r"));
  return "";
}

bool SikwiWifi::deleteData(const char * name){
  this->writeCharPROGMEM(PSTR("SK+DATA_D="));
  this->writeCharPROGMEMln(name);
  return this->wait_for_esp_response(1000);
}

bool SikwiWifi::enableHistoryFor(const char * name){
  this->writeCharPROGMEM(PSTR("SK+DATA_H="));
  this->writeCharPROGMEMln(name);
  return this->wait_for_esp_response(1000);
}

bool SikwiWifi::setMaxHistoryItem(uint8_t number){
  this->writeCharPROGMEM(PSTR("SK+DATA_M="));
  this->espSerial->println(number);
  return this->wait_for_esp_response(1000);
}

/*************************************************************************************/
/**************************************** Process ************************************/
/*************************************************************************************/

bool SikwiWifi::process()
{
  newLine = this->read_till_eol();
  if(newLine){
    if(strncmp_P(this->buffer, PSTR("SK+RDY"), 6)==0 || strncmp_P(this->buffer, PSTR("Exception"), 9)==0 || strncmp_P(this->buffer, PSTR(">>>st"), 5)==0)
      return false;

    this->previousMillis = millis();
    return true;
  }
  else if(millis() - this->previousMillis >= this->intervalTest) {
    this->previousMillis = millis();
    uint8_t i = 0;
    do{
      this->writeCharPROGMEMln(PSTR("SK"));
      if(this->wait_for_esp_response(1000))
        return true;
      delay(300);
    }while(i++<3);

    return false;
  }

  return true;
}

/**************************************************************************************/
/**************************************** WRITE ***************************************/
/**************************************************************************************/

void SikwiWifi::writeCharPROGMEM(const char *signMessage){
  char myChar;
  uint8_t k;
  uint8_t len = strlen_P(signMessage);
  for (k = 0; k < len; k++)
  {
    myChar =  pgm_read_byte_near(signMessage + k);
    this->espSerial->write(myChar);
    //this->debugPrinter->write(myChar);
  }
}

void SikwiWifi::writeCharPROGMEMln(const char *signMessage)
{
  this->writeCharPROGMEM(signMessage);
  this->espSerial->println();
  //this->debugPrinter->println();
}


/**************************************************************************************/
/**************************************** Read ****************************************/
/**************************************************************************************/

bool SikwiWifi::read_till_eol() {
  static uint8_t i=0;
  if(this->espSerial->available()) {
    this->buffer[i++]=this->espSerial->read();
    if(i==BUFFER_SIZE)  i=0;
    if(i>1 && this->buffer[i-2]==13 && this->buffer[i-1]==10) {
      this->buffer[i]=0;
      i=0;
      #if defined(DEBUG_WIFI)
        this->debugPrinter->print(this->buffer);
      #endif
      return true;
    }
  }
  return false;
}

bool SikwiWifi::wait_for_esp_response(const unsigned long timeout)
{
  return this->wait_for_esp_response(timeout, PSTR("OK\r\n"));
}

bool SikwiWifi::wait_for_esp_response(const unsigned long timeout, const char* term)
{
  unsigned long t=millis();
  bool found=false;
  char len=strlen_P(term);
  uint8_t i=0;
  while(millis()<t+timeout) {
    if(this->espSerial->available()){
      char myChar = (char)this->espSerial->read();
      this->buffer[i++]=myChar;
      if(i>=len) {
        if(strncmp_P(this->buffer+i-len, term, len)==0) {
          found=true;
          break;
        }
      }
    }
  }

  this->buffer[i]=0;
  #if defined(DEBUG_WIFI)
    this->debugPrinter->println(this->buffer);
  #endif
  return found;
}
