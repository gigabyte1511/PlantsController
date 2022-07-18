
//Отправка показаний датчиков на облако ----------------------------------------------------------
void sendDataFromSensors(){
  
  uint64_t timer_val;
  timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_val);
  
  if (timer_val >= (sendDataFromSensorsTimerValue + SEND_DATA_FROM_SENSORS_TIMER_PERIOD) ){
    HTTPClient ThingSpeak_Statistic;
    const String ENDPOINT = "https://api.thingspeak.com/update?api_key=4FQJV1HXPOH46XFD&field1=" + String(getLightMeter())+
                                                                                      "&field2=" + String(getSoilHumidity())+
                                                                                      "&field3=" + String(getDHT11_Temperature())+
                                                                                      "&field4=" + String(getDHT11_Humidity());
    Serial.println(ENDPOINT);
    ThingSpeak_Statistic.begin(ENDPOINT);
    int httpCode =  ThingSpeak_Statistic.GET();
    String payload =  ThingSpeak_Statistic.getString();                  //Получите полезную нагрузку ответа
    Serial.println(httpCode);   //Распечатать код возврата HTTP
    Serial.println(payload);    //Полезная нагрузка для ответа на запрос печати
    ThingSpeak_Statistic.end();  //Закрыть соединение
    sendDataFromSensorsTimerValue = timer_val;  
  }
  
}
//Функция получения внешнего IP-адреса ----------------------------------------------------------
String getIP(){
  HTTPClient getIP;
  const String ENDPOINT = "http://ipv4.internet.yandex.net/internet/api/v0/ip";
  Serial.println(ENDPOINT);
  getIP.begin(ENDPOINT);
  int httpCode =  getIP.GET();
  String payload =  getIP.getString();                  //Получите полезную нагрузку ответа
  Serial.println(httpCode);   //Распечатать код возврата HTTP
  Serial.println(payload);    //Полезная нагрузка для ответа на запрос печати
  getIP.end();  //Закрыть соединение

  return payload;
  
}
