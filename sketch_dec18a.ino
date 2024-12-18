#include <TroykaMQ.h> 
#include "DHT.h" 
#include <Adafruit_Sensor.h> 
#define DHTPIN 2 
#define PIN_MQ135 A0 
#define PIN_MQ135_HEATER 11 
const int speakerPin = 8;  // Пин, к которому подключен спикер 
const int sensorPin = A0;  // Пин аналогового датчика
const int threshold = 1000; // Пороговое значение для активации спикера
DHT dht(DHTPIN, DHT11); // Инициация датчика 
MQ135 mq135(PIN_MQ135); 
 
void sendDataToSerial(float humidity, float temperature, float co2, int sensorValue) { 
  // Отправка данных в формате: влажность,температура,co2,значение_сенсора 
  Serial.print(humidity); 
  Serial.print(","); 
  Serial.print(temperature); 
  Serial.print(","); 
  Serial.print(co2); 
  Serial.print(","); 
  Serial.println(sensorValue); 
} 
 
void setup() { 
  // открываем последовательный порт 
  Serial.begin(9600); 
  dht.begin(); // Инициализация DHT11 
  mq135.heaterPwrHigh(); // Включаем нагреватель 
  Serial.println("Heated sensor"); 
  pinMode(speakerPin, OUTPUT); 
} 
 
void loop() { 
  delay(2000); // 2 секунды задержки 
  float h = dht.readHumidity(); // Измеряем влажность 
  float t = dht.readTemperature(); // Измеряем температуру 
  if (isnan(h) || isnan(t)) { 
    Serial.println("Ошибка считывания"); 
    return; 
  } 
  // если прошёл интервал нагрева датчика
  // и калибровка не была совершена
  if (!mq135.isCalibrated() && mq135.heatingCompleted()) {
    // выполняем калибровку датчика на чистом воздухе 
    mq135.calibrate(); 
    Serial.print("Ro = "); 
    Serial.println(mq135.getRo()); 
  } 
  // если прошёл интевал нагрева датчика
  // и калибровка была совершена
  if (mq135.isCalibrated() && mq135.heatingCompleted()) { 
    float ratio = mq135.readRatio(); 
    float co2 = mq135.readCO2(); 
 
    Serial.print("Ratio: "); 
    Serial.print(ratio); 
    Serial.print("\tCO2: "); 
    Serial.print(co2); 
    Serial.println(" ppm"); 
 
    Serial.print("Влажность: "); 
    Serial.print(h); 
    Serial.print(" %\t"); 
    Serial.print("Температура: "); 
    Serial.print(t); 
    Serial.println(" *C "); 
 
    // Считываем значение с аналогового датчика 
    int sensorValue = analogRead(sensorPin); 
 
    Serial.print("Sensor Value: "); 
    Serial.println(sensorValue); 
 
    // Отправляем данные в Python 
    sendDataToSerial(h, t, co2, sensorValue); 
 
    // Проверка на превышение порога 
    if (sensorValue >= threshold) { 
      tone(speakerPin, 1000); // Генерируем звук на частоте 1000 Гц 
      delay(100); 
      noTone(speakerPin); 
      delay(500); 
    } else { 
      noTone(speakerPin); 
    } 
    delay(100); 
  } 
}