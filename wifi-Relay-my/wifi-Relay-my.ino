
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 13
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
#include <ESP8266WiFi.h>
#include <Wtv020sd16p.h>

#define RELON LOW
#define RELOFF HIGH

// Имя и пароль вашей сети WiFi
const char* ssid = "Oleg";
const char* password = "9111189680";
uint32_t delayMS;
int rele = 12;
int resetPin = 5; //подключение ножки сброса модуля к пину 6
int clockPin = 0; //подключение ножки тактового сигнала модуля к пину 7
int dataPin = 4;  //подключение информационного вывода модуля к пину 5
int busyPin = 14;  //подключение вывода "занятости" модуля к пину 8

Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);
// Создаем сервер и порт для прослушки 80

WiFiServer server(80);

void setup() {

  Serial.begin(115200);
  delay(10);
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  // Подготовка GPIO
//  pinMode(5, OUTPUT);
//  digitalWrite(5, 0);

  pinMode(rele, OUTPUT);
  digitalWrite(rele,RELOFF);
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);

  wtv020sd16p.reset();
  randomSeed(analogRead(0));

  // Подключаемся к сети WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Ожидание подключения
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Запуск сервера
  server.begin();
  Serial.println("Server started");

  // Вывод полученного IP адреса
  Serial.println(WiFi.localIP());
  dht.begin();
}

void loop() {

  // Проверка подключения
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Ожидание данных
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Чтение первой строки запроса
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Работа с GPIO
  if (req.indexOf("/on1") != -1){
    digitalWrite(2, 1);
    wtv020sd16p.playVoice(3);
    delay(3000);
    }
    
  if (req.indexOf("/off1") != -1){
    digitalWrite(2, 0);
    wtv020sd16p.playVoice(3);
    delay(3000);
    }
  if (req.indexOf("/on2") != -1){
    digitalWrite(rele, RELON);
    wtv020sd16p.playVoice(3);
    delay(3000);
    }
    
  if (req.indexOf("/off2") != -1){
    digitalWrite(rele, RELOFF);
    wtv020sd16p.playVoice(3);
    delay(3000);
    }
  if (req.indexOf("/allon") != -1){
    digitalWrite(rele, RELON);
    digitalWrite(2, 1);
    wtv020sd16p.playVoice(3);
    delay(3000);
    }
    
  if (req.indexOf("/alloff") != -1){
    digitalWrite(rele, RELOFF);
    digitalWrite(2, 0);
    wtv020sd16p.playVoice(3);
    delay(3000);
    }
  if (req.indexOf("/hello") != -1){
     int a = 100 + random(1, 5);
      wtv020sd16p.playVoice(a);
      delay(3000);
    }
    if (req.indexOf("/bye") != -1){
      int a = 200 + random(1, 6);
      wtv020sd16p.playVoice(a);
      delay(3000);
    }
    if (req.indexOf("/good") != -1){
      wtv020sd16p.playVoice(6);
      delay(3000);
    }
    if (req.indexOf("/bad") != -1){
      wtv020sd16p.playVoice(7);
      delay(3000);
    }
    if (req.indexOf("/hru") != -1){
      wtv020sd16p.playVoice(5);
      delay(3000);
    }
  if (req.indexOf("/temp") != -1){


//    int temp = dht.readTemperature();
     sensors_event_t event;  
     dht.temperature().getEvent(&event);
//    Serial.print(temp);
//     Serial.println(temp+8000);

      wtv020sd16p.playVoice(event.temperature + 8000);
//     wtv020sd16p.playVoice(temp + 8000);
     delay(2000);

      
    }

  if (req.indexOf("/test") != -1) {
    Serial.println("TEST OK");
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nTest OK. Uptime: ";

    // Подстчет UpTime
    int  Sec   = (millis() / 1000UL) % 60;
    int  Min   = ((millis() / 1000UL) / 60UL) % 60;
    int  Hours = ((millis() / 1000UL) / 3600UL) % 24;
    int  Day   = ((millis() / 1000UL) / 3600UL / 24UL);
    s += Day;
    s += "d ";
    s += Hours;
    s += ":";
    s += Min;
    s += ":";
    s += Sec;
    s += "</html>\n";
    client.print(s);
    client.stop();
    return;
  }
  else
    // Если неверный запрос написать об ошибке
  {
    Serial.println("invalid request");
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nInvalid request";
    s += "</html>\n";
    client.print(s);
    client.stop();
    return;
  }

  client.flush();



  // Формирование ответа
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO set OK";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

}

