//--функция определения показания датчика освещенности-------------
int getLightMeter(){
  BH1750 lightMeter;
  Wire.begin();
  lightMeter.begin();
  int value = lightMeter.readLightLevel();
  return value;
}

//--функция определения показания датчика влажности почвы-------------
int getSoilHumidity(){
  const int moisturePin = 34;
  int value = map(analogRead(moisturePin), 1100, 2900, 100, 1);
  return value;
}

//--функция определения показания датчика температуры-------------
int getDHT11_Temperature(){
 #define DHTTYPE DHT11
  uint8_t DHTPin = 4;
  DHT dht(DHTPin, DHTTYPE);
  dht.begin();
  //int value = dht.readTemperature();
  int value = 23;
  return value;
}

//--функция определения показания датчика влажности воздуха-------------
int getDHT11_Humidity(){
 #define DHTTYPE DHT11
  uint8_t DHTPin = 4;
  DHT dht(DHTPin, DHTTYPE);
  dht.begin();
  //int value = dht.readHumidity();
  int value = 70;
  return value;
}


void tasksPeripheryControl(){
  
  int fanHoodFlag = 0;
  int fanBlowingFlag = 0;
  int lightFlag = 0;
  int hydrationFlag = 0;
  int wateringFlag = 0;
  
//Вытяжка--------------------------------------------------------------
  for(int i = 0; i<= (numberOfTasks - 1); i++){
    fanHoodFlag = fanHoodFlag + fanHood[i];
  }
  if (fanHood[15] == 2){digitalWrite(25, LOW);}
  else if (fanHood[15] == 1){digitalWrite(25, HIGH);}
  else if (fanHoodFlag > 0){digitalWrite(25, LOW);}
  else digitalWrite(25, HIGH);
   
//Обдув--------------------------------------------------------------
  for(int i = 0; i<= (numberOfTasks - 1); i++){
    fanBlowingFlag = fanBlowingFlag + fanBlowing[i];
  }
  if (fanBlowing[15] == 2){digitalWrite(26, LOW);}
  else if (fanBlowing[15] == 1){digitalWrite(26, HIGH);}
  else if (fanBlowingFlag > 0){digitalWrite(26, LOW);}
  else digitalWrite(26, HIGH);
    
//Освещение--------------------------------------------------------------
  for(int i = 0; i<= (numberOfTasks - 1); i++){
    lightFlag = lightFlag + light[i];
  }
   if (light[15] == 2){digitalWrite(27, LOW);}
  else if (light[15] == 1){digitalWrite(27, HIGH);}
  else if (lightFlag > 0){digitalWrite(27, LOW);}
  else digitalWrite(27, HIGH);
    
//Увлажнение--------------------------------------------------------------
  for(int i = 0; i<= (numberOfTasks - 1); i++){
    hydrationFlag = hydrationFlag + hydration[i];
  }
  if (hydration[15] == 2){digitalWrite(14, LOW);}
  else if (hydration[15] == 1){digitalWrite(14, HIGH);}
  else if (hydrationFlag > 0){digitalWrite(14, LOW);}
  else digitalWrite(14, HIGH);
    
//Полив--------------------------------------------------------------
  for(int i = 0; i<= (numberOfTasks - 1); i++){
    wateringFlag = wateringFlag + watering[i];
  }
  if ((wateringFlag + watering[14]) > 0){
    //Serial.println("watering RUN!"); 
    //digitalWrite(26, LOW);
  }
  else{
    //Serial.println("watering STOP!");
    //digitalWrite(26, HIGH);
  }
 //Serial.println("--------------------------------");
}
