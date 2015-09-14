#include "SikwiWifiAccessPointConfiguration.h"

SikwiWifiAccessPointConfiguration::SikwiWifiAccessPointConfiguration(SikwiWifi *wifi)
{
  this->wifi = wifi;
}

/******************************************************************************************/
/**************************************** Setup Server ************************************/
/******************************************************************************************/

bool SikwiWifiAccessPointConfiguration::connect()
{
  this->wifi->writeCharPROGMEMln(PSTR("SK+WCJAP"));
  return this->waitForConnexion();
}



bool SikwiWifiAccessPointConfiguration::connect(const char *APName)
{
  this->wifi->writeCharPROGMEM(PSTR("SK+WCJAP="));
  this->wifi->writeCharPROGMEMln(APName);
  return this->waitForConnexion();
}

bool SikwiWifiAccessPointConfiguration::waitForConnexion(){
  if(this->wifi->wait_for_esp_response(40000))
  {
    this->connectedToWifi = false;

    char *result = this->wifi->buffer;/*strtok(this->wifi->buffer, "\r")*/;
    Serial.println(result);
    char *line = strstr_P(result, PSTR("SSID"));
    if(strlen(line)>1){

      this->wifi->debugPrinter->print(F("Create a "));
      this->wifi->debugPrinter->println(line);

      while(true){
        if(this->process()){
          this->wifi->reset();
          return this->connect();
        }
        delay(1);
      }
    }

    line = strstr_P(result, PSTR("IP:"));
    if(strlen(line)>1){
      this->connectedToWifi = true;

      this->wifi->debugPrinter->print(F("connected with "));
      this->wifi->debugPrinter->println(line);
      return true;
    }
    else{
      return false;
    }
  }else{
    return false;
  }

  return true;
}

bool SikwiWifiAccessPointConfiguration::process()
{
  if(this->connectedToWifi == false){
    int milliseconds = millis()%1000;
    if(milliseconds<150)
      digitalWrite(this->wifi->debugPin, HIGH);
    else if(milliseconds<300)
      digitalWrite(this->wifi->debugPin, LOW);
    else if(milliseconds<450)
      digitalWrite(this->wifi->debugPin, HIGH);
    else if(milliseconds<600)
      digitalWrite(this->wifi->debugPin, LOW);
  }

  this->wifi->newLine = this->wifi->read_till_eol();
  if(this->wifi->newLine){
    if(strncmp_P(this->wifi->buffer, PSTR("SK+RDY"), 6)==0 || strncmp_P(this->wifi->buffer, PSTR("Exception"), 9)==0 || strncmp_P(this->wifi->buffer, PSTR(">>>st"), 5)==0)
      return true;

    if(strncmp_P(this->wifi->buffer, PSTR("ERROR"), 5)==0)
      return true;

    this->wifi->previousMillis = millis();
    return false;
  }

  return false;
}

/*************************************************************************************/
/**************************************** Process ************************************/
/*************************************************************************************/
