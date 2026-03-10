#include <Servo.h>
#include <WiFi.h>
#include "DFRobot_BME280.h"
#include <Wire.h>

const char* ssid = "NAME";
const char* password = "PASSWORD";

DFRobot_BME280 bme(0x77);

Servo pan;
Servo tilt;

int pirState1 = LOW;
int pirState2 = LOW;
int pirState3 = LOW;

void setup_wifi();
void motionDetect();
void motormove(int posPan, int posTilt);
void bme280();

void setup()
{
  Serial.begin(115200);

  Wire.setSCL(19);
  Wire.setSDA(18);

  bme.begin();

  pan.attach(37);
  tilt.attach(38);
  pan.write(0);
  tilt.write(0);

  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  setup_wifi();

  Serial.println("Warming up PIR sensor...");
  delay(45000);
  Serial.println("Ready to detect motion!");
}

void loop()
{
  bme280();
  motionDetect();
}

void setup_wifi()
{
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  WiFi.status() == WL_CONNECTED);
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void motionDetect()
{
  int val1 = digitalRead(6);
  int val2 = digitalRead(7);
  int val3 = digitalRead(8);

  if (val1 == HIGH)
  {
    Serial.println("Motion detected (PIR 1)");
    pirState1 = val1;
    motormove(90, 90);
  }
  else if (val2 == HIGH)
  {
    Serial.println("Motion detected (PIR 2)");
    pirState2 = val2;
    motormove(90, 90);
  }
  else if (val3 == HIGH)
  {
    Serial.println("Motion detected (PIR 3)");
    pirState3 = val3;
    motormove(90, 90);
  }
  else
  {
    Serial.println("No motion detected!");
  }
}

void motormove(int posPan, int posTilt)
{
  pan.write(posPan);
  tilt.write(posTilt);
  delay(500);
}

void bme280()
{
  Serial.print("Temp: ");
  Serial.print(bme.getTemperature() / 100.0);
  Serial.print(" °C  ");

  Serial.print("Pressure: ");
  Serial.print(bme.getPressure() / 100.0);
  Serial.print(" hPa  ");

  Serial.print("Humidity: ");
  Serial.print(bme.getHumidity() / 1000.0);
  Serial.println(" %");

  delay(500);
}
