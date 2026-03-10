#include <Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DFRobot_BME280.h"
#include <Wire.h>

const char* ssid="XYZ";
const char* password="45362573";
const char* mqtt_server="192.168.29.151";
const char* topic_motion="farm/motion";
const char* topic_env="farm/environment";

WiFiClient espClient;
PubSubClient client(espClient);

DFRobot_BME280 bme(0x77);

Servo pan;
Servo tilt;

int pirState1=LOW;
int pirState2=LOW;
int pirState3=LOW;

int pir1=6;
int pir2=7;
int pir3=8;

void setup()
{
	Serial.begin(115200);
	Wire.setSCL(19);
  Wire.setSDA(18);
  Wire.begin();
  bme.begin();

	pan.attach(37);
	tilt.attach(38);
	pan.write(90);
	tilt.write(90);

	pinMode(pir1,INPUT);
	pinMode(pir2,INPUT);
	pinMode(pir3,INPUT);

	setup_wifi();
	client.setServer(mqtt_server,1883);
  
	Serial.println("Warming up PIR sensors...");
	delay(45000);
	Serial.println("Ready to detect motion!");
}

void loop()
{
	if(!client.connected())
	{
		reconnect();
	}
	client.loop();

	bme280_read();
	motionDetect();
}

void setup_wifi()
{
	Serial.println("Connecting to WiFi...");
	WiFi.begin(ssid,password);
	while(WiFi.status()!=WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("\nWiFi Connected!");
	Serial.println(WiFi.localIP());
}

void reconnect()
{
	while(!client.connected())
	{
		Serial.println("Connecting to MQTT...");
		if(client.connect("ESP32_PIR"))
		{
			Serial.println("MQTT Connected");
		}
		else
		{
			Serial.println("MQTT Failed. Retrying...");
			delay(2000);
		}
	}
}

void motionDetect()
{
	int val1=digitalRead(pir1);
	int val2=digitalRead(pir2);
	int val3=digitalRead(pir3);
	if(val1==HIGH&&pirState1==LOW)
	{
		Serial.println("Motion detected (PIR 1)");
		client.publish(topic_motion,"PIR1 MOTION");
		pirState1=HIGH;
		motormove(45,90);
	}
	else if(val2==HIGH&&pirState2==LOW)
	{
		Serial.println("Motion detected (PIR 2)");
		client.publish(topic_motion,"PIR2 MOTION");
		pirState2=HIGH;
		motormove(90,90);
	}
	else if(val3==HIGH&&pirState3==LOW)
	{
		Serial.println("Motion detected (PIR 3)");
		client.publish(topic_motion,"PIR3 MOTION");
		pirState3=HIGH;
		motormove(135,90);
	}
	if(val1==LOW)pirState1=LOW;
	if(val2==LOW)pirState2=LOW;
	if(val3==LOW)pirState3=LOW;
}

void motormove(int posPan,int posTilt)
{
	pan.write(posPan);
	tilt.write(posTilt);
	delay(500);
}

void bme280_read()
{
	float temp=bme.getTemperature();
	float pressure=bme.getPressure();
	float humidity=bme.getHumidity();
  
	Serial.print("Temp: ");
	Serial.print(temp);
	Serial.print(" C ");

	Serial.print("Pressure: ");
	Serial.print(pressure);
	Serial.print(" hPa ");

	Serial.print("Humidity: ");
	Serial.print(humidity);
	Serial.println(" %");

	String payload=String(temp)+","+String(pressure)+","+String(humidity);
	client.publish(topic_env,payload.c_str());
	delay(500);
}
