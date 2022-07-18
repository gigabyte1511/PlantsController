//https://russianblogs.com/article/89122151477/
//http://arduino.esp8266.com/Arduino/versions/2.0.0/doc/filesystem.html
//https://wikihandbk.com/wiki/ESP8266:%D0%9F%D1%80%D0%BE%D1%88%D0%B8%D0%B2%D0%BA%D0%B8/Arduino/%D0%A0%D0%B0%D0%B1%D0%BE%D1%82%D0%B0_%D1%81_%D1%84%D0%B0%D0%B9%D0%BB%D0%BE%D0%B2%D0%BE%D0%B9_%D1%81%D0%B8%D1%81%D1%82%D0%B5%D0%BC%D0%BE%D0%B9_%D0%B2_%D0%B0%D0%B4%D0%B4%D0%BE%D0%BD%D0%B5_ESP8266_%D0%B4%D0%BB%D1%8F_IDE_Arduino#.D0.9E.D0.B1.D1.8A.D0.B5.D0.BA.D1.82_.D1.84.D0.B0.D0.B9.D0.BB.D0.BE.D0.B2.D0.BE.D0.B9_.D1.81.D0.B8.D1.81.D1.82.D0.B5.D0.BC.D1.8B_.28SPIFFS.29

//--функция чтения значения параметра(ключа) из памяти микроконтроллера-----------
int SPIFFS_JsonReadParams(String key){
  //Read from SPIFFS:
  File file = SPIFFS.open("/Params.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return 0;
  }
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error){
    Serial.println(F("Failed to read file, using default configuration"));
  }
  file.close();
  int value = doc[key];  
  return value;
}

//--функция записи значения параметра(ключа) в память микроконтроллера-------------
void SPIFFS_JsonWriteParams(String key, int value){ 
  //Read from SPIFFS:
  File file = SPIFFS.open("/Params.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  } 
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error){
    Serial.println(F("Failed to read file, using default configuration"));
    return;
  }
  file.close();  
   
  //Write to SPIFFS:
  file = SPIFFS.open("/Params.json", "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  doc[key] = value;
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  file.close(); 
} 

//-------------------------------------------------------------------------------

 void SPIFFS_JsonReadTask(String key){
  //Read from SPIFFS:
  File file = SPIFFS.open("/Tasks.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    
  }
  //while (file.available()) {
  //  Serial.write(file.read());
  //}
 
  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, file);
  serializeJson(doc, Serial);
  //Serial.println(a);
  //serializeJson(doc, Serial);
 //String a = doc[0];
 //String b = doc[0]["from"];
 //Serial.println(a);
 //Serial.println(b);
 //Serial.print(c);
 Serial.println(" ");
  Serial.println("----------------------------------------------");
  if (error){
    Serial.println(F("Failed to read file, using default configuration"));
  }
  file.close();
 // int value = doc[key];  
  //serializeJson(doc, String a);
  //Serial.println(a);
}

//-------------------------------------------------------------------------------
 void SPIFFS_doTask(){
  //Read from SPIFFS:
  File file = SPIFFS.open("/Tasks.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    
  }
  //while (file.available()) {
  //  Serial.write(file.read());
  //}
 
  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, file);
  //String a = doc["object"];
  //Serial.println(a);
  //serializeJson(doc, Serial);
 numberOfTasks =doc[0]["numberOfTasks"];
 //String objectMass[numberOfTasks];
 //unsigned long fromMass[numberOfTasks];
 //unsigned long toMass[numberOfTasks];

 memset(objectMass, 0, sizeof(objectMass));
 memset(fromMass, 0, sizeof(fromMass));
 memset(toMass, 0, sizeof(toMass));
 
 memset(fanHood, 0, maxTasks-1);
 memset(fanBlowing, 0, maxTasks-1);
 memset(light, 0, maxTasks-1);
 memset(hydration, 0, maxTasks-1);
 memset(watering, 0, maxTasks-1);
     
 for (int i = 0; i<= numberOfTasks ; i++ ){
  String objectStr = doc[i]["object"];
  objectMass[i] = objectStr;
  
  String fromStr = doc[i]["from"];
  fromMass[i] = timeTransformToSec(fromStr);
  
  String toStr = doc[i]["to"];
  toMass[i] = timeTransformToSec(toStr);

 }
  //Serial.println("doTask");
  //Serial.println(objectMass[0]);
  //Serial.println(fromMass[0]);
  //Serial.println(toMass[0]);
  //Serial.println("--------------");
  
  if (error){
    Serial.println(F("SPIFFS_doTask: Failed to read file, using default configuration"));
  }
  file.close();

}

//-------------------------------------------------------------------------------
void SPIFFS_JsonWriteStatisticEvents(String Mode,char Date[], String Time, String Performer, String Event){ 
  //Read from SPIFFS:
  if (Mode == "ADD"){
    File file = SPIFFS.open("/Statistic_Events.json", "a");
    if(!file){
      Serial.println("Failed to open file for reading");
      return;
    } 
    String a = "{\"Type\":\"Event\",\"Time\":\"" +Time+"\"," + "\"Performer\":\"" + Performer+"\"," + "\"Date\":\"" + Date +"\"," + "\"Event\":\""+ Event +"\"" + "},"; 
    //String a = "{\"Type\":\"Event\",\"Time\":\"" +Time+"\"," + "\"Performer\":\"" + Performer+"\"," + "\"Event\":\""+ Event +"\"" + "},"; 
    file.print(a); 
    file.close(); 
  }
  if (Mode == "SEND"){
    File file = SPIFFS.open("/Statistic_Events.json", "a");
    if(!file){
      Serial.println("Failed to open file for reading");
      return;
    } 
    file.print("{\"Type\":\"Event\",\"Time\":\"" +Time+"\"," + "\"Performer\":\"" + Performer+"\"," + "\"Date\":\"" + Date +"\"," + "\"Event\":\""+ Event +"\"" + "}]");
    file.close(); 
    
    file = SPIFFS.open("/Statistic_Events.json", "r");
    if(!file){
      Serial.println("Failed to open file for reading");  
    }
    StaticJsonDocument<1024> stats;   
    deserializeJson(stats, file);
    String dataStats;
    serializeJson(stats, dataStats);
    SPIFFS_JsonReadStatisticEventsBytes();
    HTTPClient GoogleAppsScript_Statistic;//Объявить объект класса HttpClient 
    
    GoogleAppsScript_Statistic.begin("https://script.google.com/macros/s/AKfycbyJrLD4KxdkzhDByzvXsXs-xnO6XwAVi6SbnASFBBx9nXRl9tECRy2vZV0P-F87q7EW9Q/exec");      //Укажите адрес запроса
    GoogleAppsScript_Statistic.addHeader("Content-Type", "text/plain");  //Укажите заголовок типа содержимого
    int httpCode = GoogleAppsScript_Statistic.POST(dataStats);  //Отправьте запрос
    //int httpCode = GoogleAppsScript_Statistic.sendRequest("POST", file, 0);
    String payload = GoogleAppsScript_Statistic.getString();    //Получите полезную нагрузку ответа
    Serial.println(httpCode);   //Распечатать код возврата HTTP
    Serial.println(payload);    //Полезная нагрузка для ответа на запрос печати
    GoogleAppsScript_Statistic.end();  //Закрыть соединение
    file.close(); 

    file = SPIFFS.open("/Statistic_Events.json", "w");
    if(!file){
      Serial.println("Failed to open file for reading");
      return;
    } 
    file.print("[");
    file.close(); 
    
  }
} 

//-------------------------------------------------------------------------------

 void SPIFFS_JsonReadStatisticEvents(){
  //Read from SPIFFS:
  File file = SPIFFS.open("/Statistic_Events.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");  
  }
  //while (file.available()) {
  //  Serial.write(file.read());
  //}
 
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  serializeJson(doc, Serial);
  //Serial.println(a);
  //serializeJson(doc, Serial);
 //String a = doc[0];
 //String b = doc[0]["from"];
 //Serial.println(a);
 //Serial.println(b);
 //Serial.print(c);
 Serial.println(" ");
  Serial.println("----------------------------------------------");
  if (error){
    Serial.println(F("Failed to read file, using default configuration"));
  }
  file.close();
 // int value = doc[key];  
  //serializeJson(doc, String a);
  //Serial.println(a);
}
void SPIFFS_JsonReadStatisticEventsBytes(){
  //Read from SPIFFS:
  File file = SPIFFS.open("/Statistic_Events.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");  
  }
  //while (file.available()) {
  //  Serial.write(file.read());
  //}
 
  while(file.available()) {
  Serial.write(file.read());
  }
  file.close();
 // int value = doc[key];  
  //serializeJson(doc, String a);
  //Serial.println(a);
}



void getLightMeterSPIFFS(){
 
  File file = SPIFFS.open("/Params.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  } 
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error){
    Serial.println(F("Failed to read file, using default configuration"));
    return;
  }
  file.close();  
   
  //Write to SPIFFS:
  file = SPIFFS.open("/Params.json", "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  doc["Light"] = getLightMeter();
  
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file")); 
  }
  file.close(); 
}

//--функция записи показания датчика влажности почвы в память микроконтроллера-------------
void getSoilHumiditySPIFFS(){
  
  File file = SPIFFS.open("/Params.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  } 
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error){
    Serial.println(F("Failed to read file, using default configuration"));
    return;
  }
  file.close();  
   
  //Write to SPIFFS:
  file = SPIFFS.open("/Params.json", "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  
  doc["SoilHumidity"] = getSoilHumidity();
  
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file")); 
  }
  file.close(); 
}

//--функция записи показаний датчика DHT11 в память микроконтроллера-------------
void getDHT11SPIFFS(){

  File file = SPIFFS.open("/Params.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  } 
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error){
    Serial.println(F("Failed to read file, using default configuration"));
    return;
  }
  file.close();  
   
  //Write to SPIFFS:
  file = SPIFFS.open("/Params.json", "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  
  doc["TemperatureDHT11"] = getDHT11_Temperature();
  doc["HumidityDHT11"] = getDHT11_Humidity();
  
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  file.close(); 
}
