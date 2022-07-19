//https://raphaelpralat.medium.com/example-of-json-rest-api-for-esp32-4a5f64774a05
//https://github.com/me-no-dev/ESPAsyncWebServer/issues/195

void AsyncWebServer(){

  // URL для корневой страницы веб-сервера:---------------------------------------------------------------
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(HTTP_USERNAME, HTTTP_PASSWORD)){
      return request->requestAuthentication();
    }
    request->send(SPIFFS, "/index.html", String(), false, processor);
    //Запись события: Подключение клиента ---------------------------
    SPIFFS_JsonWriteStatisticEvents("ADD",dateString,getTimefromTimerString(),"Сервер","Подключение клиента");
  });
  
  // URL для файла «style.css»:---------------------------------------------------------------
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // URL для API веб-сервера, отправка клиенту JSON Params SPIFFS:-----------------------------------------------
  server.on("/api/read", HTTP_POST, [](AsyncWebServerRequest *request){
   //Опрос всех датчиков  и запись их показаний в Params.json
   getDHT11SPIFFS();
   getSoilHumiditySPIFFS();
   getLightMeterSPIFFS();
   request->send(SPIFFS, "/Params.json", String(), false, processor);
 
  });

 
  // URL для API веб-сервера, получение и парсинг JSON Params и Tasks от клиента:
    //POST Params:
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if ((request->url() == "/api/write") && (request->method() == HTTP_POST)){
      const size_t        JSON_DOC_SIZE   = 512U;
      DynamicJsonDocument jsonDoc(JSON_DOC_SIZE);
      if (DeserializationError::Ok == deserializeJson(jsonDoc, (const char*)data)){
        int dew = jsonDoc["Ventilation"];
        SPIFFS_JsonWriteParams("Ventilation", jsonDoc["Ventilation"]);
        SPIFFS_JsonWriteParams("Blowing", jsonDoc["Blowing"]);
        SPIFFS_JsonWriteParams("Light", jsonDoc["Light"]);
        SPIFFS_JsonWriteParams("Hydration", jsonDoc["Hydration"]); 
        }
    }
    //POST Tasks:
    if ((request->url() == "/api/tasks/write") && (request->method() == HTTP_POST)){
      const size_t        JSON_DOC_SIZE   = 2048U; 
      DynamicJsonDocument jsonDoc(JSON_DOC_SIZE);
      File file = SPIFFS.open("/Tasks.json", "w");
      if (!file) {
        Serial.println(F("Failed to create file"));
        return;
      }
      if (DeserializationError::Ok == deserializeJson(jsonDoc, (const char*)data)){
        if (serializeJson(jsonDoc, file) == 0) {
          Serial.println(F("Failed to write to file"));
        }
        file.close(); 
      }
    SPIFFS_doTask();
    }
    //POST doitnow:
     if ((request->url() == "/api/doitnow/write") && (request->method() == HTTP_POST)){
      const size_t        JSON_DOC_SIZE   = 512U;
      DynamicJsonDocument jsonDoc(JSON_DOC_SIZE);
      if (DeserializationError::Ok == deserializeJson(jsonDoc, (const char*)data)){
        if (jsonDoc["Object"]=="Вытяжка"){ fanHood[15] = jsonDoc["Status"];}
        if (jsonDoc["Object"]=="Обдув"){ fanBlowing[15] = jsonDoc["Status"];}
        if (jsonDoc["Object"]=="Освещение"){ light[15] = jsonDoc["Status"];}
        if (jsonDoc["Object"]=="Полив"){ watering[15] = jsonDoc["Status"];}
        if (jsonDoc["Object"]=="Увлажнение"){ hydration[15] = jsonDoc["Status"];}
      }
    }
    
    request->send(200, "application/json", "{ \"status\": 0 }"); 
    //Запись события: Подключены Tasks от клиента ---------------------------
    SPIFFS_JsonWriteStatisticEvents("ADD",dateString,getTimefromTimerString(),"Сервер","Получены Tasks от клиента");
   });
     
  // URL для API веб-сервера, чтение JSON Tasks из SPIFFS и отправка клиенту:---------------------------------------------------
  server.on("/api/tasks/read", HTTP_POST, [](AsyncWebServerRequest *request){
   request->send(SPIFFS, "/Tasks.json", String(), false, processor);
  //double timer_val;
  //timer_get_counter_time_sec( TIMER_GROUP_0, TIMER_1, &timer_val );
  //Serial.println(timer_val);
  
  //Запись события: Подключены Tasks от клиента ---------------------------
    SPIFFS_JsonWriteStatisticEvents("ADD",dateString,getTimefromTimerString(),"Сервер","Запрос клинта на чтение Tasks");
 
  });
  
  server.on("/api/debugging", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200);
    Serial.println("debugging");
    SPIFFS_JsonReadTask("0");
  });

  
 
}
  
