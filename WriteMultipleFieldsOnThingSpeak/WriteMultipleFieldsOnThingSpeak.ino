#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"

#define CalcAD 0.001611328125
#define Bateria 36 // A0
#define ChuvaPin 34
#define DHTPIN 4

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

float BateryPorcentage = 0;
float BateryVoltage = 0;
float Umidade;
float temperatura;
float PressureBMP = 0;
float TempBMP = 0;
float AltitudeBMP = 0;
bool rain = 0;

int number1 = 0;
int number2 = random(0, 100);
int number3 = random(0, 100);
int number4 = random(0, 100);
//String myStatus = "";

DHT dht(DHTPIN, DHT22);
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (!bmp.begin(0x76))
  {
    Serial.println(F("Sensor BMP280 não foi identificado! Verifique as conexões."));
    //while (1);
  }
  dht.begin();
  ThingSpeak.begin(client);
}

void ReadSensors()
{
  PressureBMP = (bmp.readPressure() / 100);
  TempBMP = bmp.readTemperature();
  AltitudeBMP = bmp.readAltitude(1013.25);
  temperatura = dht.readTemperature();
  Umidade = dht.readHumidity();
  rain = !digitalRead(ChuvaPin);
  BateryVoltage = analogRead(Bateria) * CalcAD;
  BateryPorcentage = map(BateryVoltage, 3.3, 4.1, 0, 100);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Tentando conectar a rede:: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(1000);
    }
    Serial.println("\nConectado!");
  }
  ReadSensors();
  ThingSpeak.setField(1, temperatura);
  ThingSpeak.setField(2, Umidade);
  ThingSpeak.setField(3, PressureBMP);
  ThingSpeak.setField(4, BateryPorcentage);
  ThingSpeak.setStatus("Tudo nos conformes");
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  ESP.deepSleep(900000000);
}
