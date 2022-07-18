//https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/
//https://www.esp32.com/viewtopic.php?t=1479

volatile int interruptCounter;
int totalInterruptCounter;
int totalInterruptCounter1;
hw_timer_t * timer = NULL;
hw_timer_t * timer1 = NULL;

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


//Функция преривание по таймеру
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
 
}
//Функция инициализации таймера
void TimerOnLoad(){

  HTTPClient http;
  const String endpoint = "http://worldtimeapi.org/api/timezone/Europe/Moscow";
  http.begin(endpoint);
  int httpCode = http.GET();
  if (httpCode > 0) { //Check for the returning code
      // выводим ответ сервера
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
      handleReceivedMessage(payload);
    }
    else {
      Serial.println("Ошибка HTTP-запроса");
    }
    http.end(); // освобождаем ресурсы микроконтроллера
  //timerAlarmWrite(timer, 2000000, true);
  //timerAlarmEnable(timer);
}

void handleReceivedMessage(String message) {
  StaticJsonDocument<1500> doc;    //Memory pool. Поставил наугад для демонстрации

  DeserializationError error = deserializeJson(doc, message);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  Serial.println();
  Serial.println("----- DATA FROM OPENWEATHER ----");

  char datetimeChar[20];
  String datetimeStr = doc["datetime"];
  char timeChar[8];
  char dateChar[10];
  datetimeStr.toCharArray(datetimeChar, 20);

  for(int i=0; i<=7; i++){
    timeChar[i]=datetimeChar[i+11];
    }
  for(int i=0; i<=9; i++){
    dateString[i]=datetimeChar[i];
    }


  Serial.print("Time: ");
  Serial.println(timeChar);
  Serial.print("TimeSec: ");
  Serial.println(timeTransformToSec(timeChar));
  //Таймер-------
  timer_config_t config;
  config.divider = 80; // тактирование счетчика 1 мкс
  config.counter_dir = TIMER_COUNT_UP; // прямой счет
  config.counter_en = TIMER_START; // счетчик работает
  config.alarm_en = TIMER_ALARM_DIS; // событие перезагрузка запрещено
  config.auto_reload = TIMER_AUTORELOAD_DIS ; // аппаратная перезагрузка запрещена
  timer_init(TIMER_GROUP_0, TIMER_0 , &config); // инициализация

  timer_init(TIMER_GROUP_0, TIMER_0, &config);
  timer_start(TIMER_GROUP_0, TIMER_0);
  
  uint64_t load_val = timeTransformToSec(timeChar);
  timer_set_counter_value( TIMER_GROUP_0, TIMER_0 , load_val);

  //Запись события: Время таймера установлено ---------------------------
  SPIFFS_JsonWriteStatisticEvents("ADD",dateString,getTimefromTimerString(),"Сервер","Время таймера установлено");
  
  Serial.println("------------------------------");
}

////////////Функция форматирования времени в секунды

uint64_t timeTransformToSec(String timeStr){

  char timeChar[9];
  timeStr.toCharArray(timeChar, 9);

  uint64_t timeSec = (timeChar[7] - '0') +
                     (timeChar[6] - '0') * 10 +
                     (timeChar[4] - '0') * 60 +
                     (timeChar[3] - '0') * 600 +
                     (timeChar[1] - '0') * 3600 +
                     (timeChar[0] - '0') * 36000;
  return timeSec * 1000000;
}
String getTimefromTimerString(){
  const uint64_t MICROSEC_PER_MILISEC =  1000;
  const uint64_t MILISEC_PER_SEC      =  1000;
  const uint64_t SEC_PER_MIN          =  60;
  const uint64_t MIN_PER_HOUR         =  60;
  
  const uint64_t MICROSEC_PER_SEC  = MICROSEC_PER_MILISEC * MILISEC_PER_SEC;
  const uint64_t MICROSEC_PER_MIN  = MICROSEC_PER_SEC * SEC_PER_MIN;
  const uint64_t MICROSEC_PER_HOUR = MICROSEC_PER_MIN * MIN_PER_HOUR;
  
  uint64_t timer_microseconds;
  timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_microseconds);
  
  int hours = timer_microseconds / MICROSEC_PER_HOUR;
  timer_microseconds %=  MICROSEC_PER_HOUR;

  int minutes = timer_microseconds / MICROSEC_PER_MIN;
  timer_microseconds %=  MICROSEC_PER_MIN;

  int seconds = timer_microseconds / MICROSEC_PER_SEC;
  timer_microseconds %=  MICROSEC_PER_SEC;

  String Time = String(hours) + ':' + String(minutes) + ':' + String(seconds); 

  return Time;
}

void timerReload(){

 uint64_t timer_val;
 timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_val);

 if (timer_val >= 86400000000 ){
  timer_set_counter_value( TIMER_GROUP_0, TIMER_0 , 0);
  TimerOnLoad();
 }
 
}
