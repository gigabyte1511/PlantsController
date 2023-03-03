// Сервер суперкомпьютера
// Импортируем необходимые библиотеки:
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#include "FS.h"                   
#include "SPIFFS.h"

#include "driver/timer.h"

#include <AsyncTCP.h>
#include <AsyncElegantOTA.h>
#include <ArduinoJson.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <BH1750.h>

#include <HTTPClient.h>;
#include <FTPServer.h>

#include <FastBot.h>

//Настройки-----------------------------------------------------------------------------------
// Вставьте ниже SSID и пароль для своей WiFi-сети:
const char* ssid = "Pyhlemo";
const char* password = "13572468";

// FTP - сервер:
const char* FTP_LOGIN = "Supercomputer";
const char* FTP_PASSWORD = "135246";

//Авторизация на странице:
#define HTTP_USERNAME "admin"
#define HTTTP_PASSWORD "135246"

#define BOT_TOKEN "5460143863:AAHcRFI42OueXaVsVzL_k28wci7SKph2qT0"    // Токен Telegram-бота

const String TIME_SEND_STATISTIC_EVENTS = "22:49:00";                 //Время отправки собранной статистики Events

const uint64_t SEND_DATA_FROM_SENSORS_TIMER_PERIOD = 180000000;       //Периодичность отправки показаний датчиков в облако, мкС.
//--------------------------------------------------------------------------------------------

//Флаги:
//int SendStatisticEventsFlag = 0; //Флаг отправки собранной статистики Events

// Создаем экземпляр класса «AsyncWebServer» под названием «server» и задаем ему номер порта «80»:
AsyncWebServer server(80);

FTPServer ftpSrv(SPIFFS);

FastBot bot(BOT_TOKEN);

#define DHTTYPE DHT11
uint8_t DHTPin = 4;
DHT dht(DHTPin, DHTTYPE);

// Таймер:
char dateString[10];
uint64_t sendDataFromSensorsTimerValue;

//Tasks:
const int maxTasks = 16;

int numberOfTasks;
String objectMass[maxTasks];
uint64_t fromMass[maxTasks];
uint64_t toMass[maxTasks];


int fanHood[maxTasks];
int fanBlowing[maxTasks];
int light[maxTasks];
int hydration[maxTasks];
int watering[maxTasks];


// Настройки шим-каналов:
//const int freq = 5000;
//const int ledChannel_0 = 0;
//const int ledChannel_1 = 1;
//const int resolution = 8;

int ledPin = 1;

/*Порядок запуска и инициализации
 * 
 * Инициализация отладочного Seial
 * Инициализация шим-каналов микроконтроллера
 * Инициализация файловой системы SPIFFS
 * Инициализация Wifi микроконтроллера
 * Установка значений выходов портов микроконтроллера
 * Запуск сервера
 * 
 */

void setup(){
  //Инициализация отладочного Seial-----------------------------------------
  Serial.begin(115200);
  
  // Инициализация шим-каналов:----------------------------------------------
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(14, OUTPUT);
  
  digitalWrite(27, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(25, HIGH);
  digitalWrite(14, HIGH);

  // Инициализация SPIFFS:--------------------------------------------------
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
               //  "При монтировании SPIFFS произошла ошибка"
    return;
  }
  // Инициализация и подключение к WiFi:------------------------------------
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
               //  "Подключаемся к WiFi..."
  }
  Serial.println(WiFi.localIP());

  //Инициализация и запуск таймера------------------------------------------
  TimerOnLoad();
  
  //Загрузка Tasks в оперативную память-------------------------------------
  SPIFFS_doTask();
  
  //Запуск AsyncWebServer---------------------------------------------------
  AsyncWebServer();
  
  //Запуск библиотеки OTA---------------------------------------------------
  AsyncElegantOTA.begin(&server);
  
  //Запуск сервера----------------------------------------------------------
  server.begin();
  
  //Запись события: Запуск системы SuperComputer ---------------------------
  //SPIFFS_JsonWriteStatisticEvents("ADD",dateString,getTimefromTimerString(),"Сервер","Запуск системы SuperComputer");

  ftpSrv.begin(F(FTP_LOGIN), F(FTP_PASSWORD));

  bot.attach(newMsg);
  bot.sendMessage("Суперкомпьютер запущен!", "554386866");
  
}
 
void loop(){
  
 AsyncElegantOTA.loop();
 ftpSrv.handleFTP();
 bot.tick();
   
  //SPIFFS_JsonReadTask("23");
 uint64_t timer_val;
 timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_val);
  for(int i = 0; i<= (numberOfTasks - 1); i++){
    
    if (fromMass[i] <= timer_val && toMass[i] >= timer_val){
      if (objectMass[i]=="Вытяжка")   { fanHood[i] = 1;} 
      if (objectMass[i]=="Обдув")     { fanBlowing[i] = 1;}
      if (objectMass[i]=="Освещение") { light[i] = 1;}
      if (objectMass[i]=="Увлажнение"){ hydration[i] = 1;}  
      if (objectMass[i]=="Полив")     { watering[i] = 1;}   
    }
    
    else{
      if (objectMass[i]=="Вытяжка")   { fanHood[i] = 0;} 
      if (objectMass[i]=="Обдув")     { fanBlowing[i] = 0;}
      if (objectMass[i]=="Освещение") { light[i] = 0;}
      if (objectMass[i]=="Увлажнение"){ hydration[i] = 0;}  
      if (objectMass[i]=="Полив")     { watering[i] = 0;}  
    }
  }

  tasksPeripheryControl();
  
  //timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_val);
  //if (timer_val > timeTransformToSec(TIME_SEND_STATISTIC_EVENTS) && SendStatisticEventsFlag == 1){
  //  SendStatisticEventsFlag=0;
  //  SPIFFS_JsonWriteStatisticEvents("SEND",dateString,getTimefromTimerString(),"Сервер","Отправка Events в GoogleApsScript");
  // }
  //if (timer_val < timeTransformToSec(TIME_SEND_STATISTIC_EVENTS)) SendStatisticEventsFlag=1;
  
  timerReload();
  //sendDataFromSensors();
 
}
//Меняем заглушку на текущее состояние светодиода:
String processor(const String& var){
  // Создаем переменную для хранения состояния светодиода:
  String ledState;

  Serial.println(var);
  if(var == "STATE1"){
    if(digitalRead(ledPin)){
      ledState = "Online";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  if(var == "STATE2"){
    if(digitalRead(ledPin)){
      ledState = "set1";
    }
    else{
      ledState = "set0";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}
