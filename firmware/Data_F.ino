aJsonObject* jsonConfigRoot;

aJsonObject* jsonDataRoot;
aJsonObject* jsonDataHistory;
String jsonDataString;

#define EEPROM_TEST_VERSION "SK0011"
#define HISTORY_NAME "hist"
#define DATE_NAME "d"
#define VALUE_NAME "v"
int HISTORY_MAX_VALUE = 5;
#define JSON_ADDR 130

void initData()
{
  String version = read_StringEE(0, 7);
  if(version != EEPROM_TEST_VERSION)
  {
    write_StringEE(0, EEPROM_TEST_VERSION);
    createJson();
  }else{
    int lengthRead;
    eeprom_read_int(JSON_ADDR, &lengthRead);
    jsonDataString = read_StringEE(JSON_ADDR+5, lengthRead);
    
    if(jsonDataString[0] != '{' || jsonDataString[jsonDataString.length()-1] != '}')
    {
      createJson();
    }else{
      unsigned int bufSize = jsonDataString.length() + 1; //String length + null terminator
      char* ret = new char[bufSize];
      jsonDataString.toCharArray(ret, bufSize);
      
      jsonDataRoot = aJson.parse(ret);
      jsonDataHistory = aJson.getObjectItem(jsonDataRoot, HISTORY_NAME);
      aJsonObject* nameObject = aJson.getObjectItem(jsonDataRoot, "name");
      if(nameObject->type == aJson_String)
        deviceName = nameObject->valuestring;
    }
  }

  jsonConfigRoot = aJson.createObject();

  aJsonObject* idObject = aJson.getObjectItem(jsonDataRoot, "uid");
  if(idObject == NULL)
    aJson.addItemToObject(jsonDataRoot, "uid", aJson.createItem(uid.c_str()));
  saveJSONToEEPROM();
}

void createJson(){
  jsonDataRoot = aJson.createObject();
  aJson.addItemToObject(jsonDataRoot, "name", aJson.createItem(deviceName.c_str()));
  aJson.addItemToObject(jsonDataRoot, HISTORY_NAME, jsonDataHistory = aJson.createObject());
}

void saveJSONToEEPROM(){
  char* res = aJson.print(jsonDataRoot);
  jsonDataString = res;
  delete(res);
  if(jsonDataString=="")
  {
    Serial.println("ERROR JSON Empty?");
    return ;
  }
    
  int length = jsonDataString.length()+1;
  eeprom_write_int(JSON_ADDR, length);
  write_StringEE(JSON_ADDR+5, jsonDataString);

  //Serial.print("change Json : ");
  //Serial.println(jsonDataString);
}

String getValueFor(String name){
  aJsonObject* result = aJson.getObjectItem(jsonDataRoot, name.c_str());
  if(result == NULL)
    return "";
  switch(result->type){
    case aJson_Boolean:{
      return String(result->valuebool);
      break;
    }
    case aJson_Int:{
      return String(result->valueint);
      break;
    }
    case aJson_Float:{
      return String(result->valuefloat);
      break;
    }
    case aJson_String:{
      return String(strdup(result->valuestring));
      break;
    }
  }
}

void changeValueStringFor(String name, String value){
  sikwiPush(name, String(value), false);
  
  aJsonObject* result = aJson.getObjectItem(jsonDataRoot, name.c_str());
  if(result == NULL)
    aJson.addItemToObject(jsonDataRoot, name.c_str(), result = aJson.createItem(value.c_str()));
  else if(result->type != aJson_String || (String)result->valuestring != value)
    result->valuestring = strdup(value.c_str());
  else
    return; 

  result->type = aJson_String;
  
  aJsonObject* resultInHistory = aJson.getObjectItem(jsonDataHistory, name.c_str());
  if(resultInHistory != NULL)
  {
    aJsonObject* entry;

    if(aJson.getArraySize(resultInHistory)==HISTORY_MAX_VALUE){
      aJson.deleteItemFromArray(resultInHistory, 0);
    }
    
    aJson.addItemToArray(resultInHistory, entry = aJson.createObject());
    aJson.addItemToObject(entry, DATE_NAME, aJson.createItem(String(getCurrentTimeStamp()).c_str()));
    aJson.addItemToObject(entry, VALUE_NAME, aJson.createItem(value.c_str()));
  }
    
  saveJSONToEEPROM();
}

void changeValueFloatFor(String name, float value){
  sikwiPush(name, String(value), false);
  
  aJsonObject* result = aJson.getObjectItem(jsonDataRoot, name.c_str());
  if(result == NULL)
    aJson.addItemToObject(jsonDataRoot, name.c_str(), result = aJson.createItem(value));
  else if(result->valuefloat != value)
    result->valuefloat = value;
  else
    return;

  result->type = aJson_Float;

  aJsonObject* resultInHistory = aJson.getObjectItem(jsonDataHistory, name.c_str());
  if(resultInHistory != NULL)
  {
    aJsonObject* entry;

    if(aJson.getArraySize(resultInHistory)==HISTORY_MAX_VALUE){
      aJson.deleteItemFromArray(resultInHistory, 0);
    }
    
    aJson.addItemToArray(resultInHistory, entry = aJson.createObject());
    aJson.addItemToObject(entry, DATE_NAME, aJson.createItem(String(getCurrentTimeStamp()).c_str()));
    aJson.addItemToObject(entry, VALUE_NAME, aJson.createItem(value));
  }
  
  saveJSONToEEPROM();
}

void changeValueIntFor(String name, int value){
  sikwiPush(name, String(value), false);
  
  aJsonObject* result = aJson.getObjectItem(jsonDataRoot, name.c_str());
  if(result == NULL)
    aJson.addItemToObject(jsonDataRoot, name.c_str(), result = aJson.createItem(value));
  else if(result->valueint != value)
    result->valueint = value;
  else
    return;

  result->type = aJson_Int;

  aJsonObject* resultInHistory = aJson.getObjectItem(jsonDataHistory, name.c_str());
  if(resultInHistory != NULL)
  {
    aJsonObject* entry;

    if(aJson.getArraySize(resultInHistory)==HISTORY_MAX_VALUE){
      aJson.deleteItemFromArray(resultInHistory, 0);
    }

    Serial.println(String(getCurrentTimeStamp()).c_str());
    aJson.addItemToArray(resultInHistory, entry = aJson.createObject());
    aJson.addItemToObject(entry, DATE_NAME, aJson.createItem(String(getCurrentTimeStamp()).c_str()));
    aJson.addItemToObject(entry, VALUE_NAME, aJson.createItem(value));
  }
  
  saveJSONToEEPROM();
}

void changeValueBoolFor(String name, bool value){
  sikwiPush(name, String(value), false);
  
  aJsonObject* result = aJson.getObjectItem(jsonDataRoot, name.c_str());
  if(result == NULL)
    aJson.addItemToObject(jsonDataRoot, name.c_str(), result = aJson.createItem(value));
  else if(result->valuebool != value)
    result->valuebool = value;
  else
    return;

  result->type = aJson_Boolean;
  
  aJsonObject* resultInHistory = aJson.getObjectItem(jsonDataHistory, name.c_str());
  if(resultInHistory != NULL)
  {
    aJsonObject* entry;

    if(aJson.getArraySize(resultInHistory)==HISTORY_MAX_VALUE){
      aJson.deleteItemFromArray(resultInHistory, 0);
    }
    
    aJson.addItemToArray(resultInHistory, entry = aJson.createObject());
    aJson.addItemToObject(entry, DATE_NAME, aJson.createItem(String(getCurrentTimeStamp()).c_str()));
    aJson.addItemToObject(entry, VALUE_NAME, aJson.createItem(value));
  }
  
  saveJSONToEEPROM();
}

void deleteValue(String name){
  aJsonObject* result = aJson.getObjectItem(jsonDataRoot, name.c_str());
  if(result != NULL)
  {
    if(name != "name" && name != HISTORY_NAME)
      aJson.deleteItemFromObject(jsonDataRoot, name.c_str());

    aJson.deleteItemFromObject(jsonDataHistory, name.c_str());
  }
  else
    return;
    
  saveJSONToEEPROM();
}

bool setHistoryForValue(String name){
  aJsonObject* result = aJson.getObjectItem(jsonDataRoot, name.c_str());
  if(result == NULL)
    return false;
    
  aJsonObject* resultInHistory = aJson.getObjectItem(jsonDataHistory, name.c_str());
  if(resultInHistory == NULL)
  {
    aJson.addItemToObject(jsonDataHistory, name.c_str(), resultInHistory = aJson.createArray());
    saveJSONToEEPROM();
  }
  return true;
}

String getConfJsonData(){
  char* res = aJson.print(jsonConfigRoot);
  String test = res;
  delete(res);
  return test;
}

