/*
* Copyright (c) 2015, Majenko Technologies
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* * Neither the name of Majenko Technologies nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include "Countimer.h"

#define ONE_WIRE_PIN 14
#define RELAY_PIN 13


const char *ssid = "Sous Vide";
const char *password = "danisousvide";


ESP8266WebServer server(80);
File file;
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
Countimer countimer;


double setpoint,
		input,
		output;

float Kp = 850,
		Ki = 0.5,
		Kd = 0.1;
int WindowSize = 10000;
unsigned long windowStartTime;
int hours, minutes;
long previousMillis = 0;

bool heatingState = false,
	restartFlag = false,
		opState;
volatile long onTime = 0;


PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

String temp;
String stateInfo;



float ReadTemperature() {
	sensors.requestTemperatures(); // Send the command to get temperatures
	float temperature = sensors.getTempCByIndex(0);
	Serial.println(temperature);
	return temperature;
}


void RunPWM() {
		if (opState == 0)
		{
			RelayState(LOW, "off");
		}
		else
		{
			if(input>=setpoint)
				RelayOperation();
		}
}

void RefreshClock() {
	Serial.print(countimer.getCurrentTime());
	Serial.print(" remain.");
}

void RelayState() {
	digitalWrite(RELAY_PIN, LOW);
	stateInfo = "Heater turned off";
}

void RelayState(bool state, String message) {
	digitalWrite(RELAY_PIN, state);
	stateInfo = "Heater turned "+message;
}

void ChangeHeatingState() {
	heatingState = !heatingState;
}

void RelayOperation() {
		unsigned long now = millis();

		if (now - windowStartTime > WindowSize)
			windowStartTime += WindowSize;

		if (output > (now - windowStartTime)) {
			RelayState(HIGH, "on");
		}
		else
		{
			RelayState(LOW, "off");
		}
	

}



void setup(void) {
	delay(1000);
	pinMode(RELAY_PIN, OUTPUT);
	Serial.begin(115200);

	//PWMSimulation.attach(0.015, RunPWM);
	sensors.begin();

	WiFi.softAP(ssid, password);
	
	//tell the PID to range between 0 and the full window size
	setpoint = 40.00;
	input = 0;

	myPID.SetOutputLimits(0, WindowSize);
	//turn the PID on
	myPID.SetMode(AUTOMATIC);

	countimer.setInterval(RefreshClock, 1000);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);
	server.on("/submit", handleSubmit);
	server.begin();
	Serial.println("HTTP server started");
	
}

void loop(void) {
	
	countimer.run();
	
	Serial.println("Input: ");
	Serial.println(input);
	Serial.print("Output: ");
	Serial.println(output);
	Serial.print("Setpoint: ");
	Serial.println(setpoint);
	Serial.print("Heating state: ");
	Serial.println(heatingState);
	Serial.println("---------");
	
	
	if (heatingState == true) {
		input = ReadTemperature();
		myPID.Compute();
		RelayOperation();
	}
	
	if (input >= setpoint) {
		countimer.start();
		if (restartFlag==true) {
			countimer.restart();
			restartFlag=false;
		}
	}

	if (countimer.isCounterCompleted())
		heatingState = restartFlag = false;

	server.handleClient();
}
