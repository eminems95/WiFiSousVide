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

#define ONE_WIRE_PIN 4
#define RELAY_PIN 5

const char *ssid = "NietetowaneWiFi";
const char *password = "PiotrSkaluba69";


ESP8266WebServer server(80);
File file;
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

double setpoint,
		input,
		output;

float Kp = 4,
		Ki = 5,
		Kd = 5;
int WindowSize = 600;
unsigned long windowStartTime;


PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

String temp;
String stateInfo;

float ReadTemperature() {
	sensors.requestTemperatures(); // Send the command to get temperatures
	float temperature = sensors.getTempCByIndex(0);
	Serial.println(temperature);
	

	return temperature;
}


void RelayOperation() {
	unsigned long now = millis();
	if (now - windowStartTime>WindowSize)
		windowStartTime += WindowSize;

	if (output > now - windowStartTime) {
		digitalWrite(RELAY_PIN, HIGH);
		stateInfo = "Heater turned on";
	}
	else {
		digitalWrite(RELAY_PIN, LOW);
		stateInfo = "Heater turned off";
	}

}

void handleRoot() {
	char temporary[2000];
	temp = "";

	/*
	SPIFFS.begin();

	file = SPIFFS.open("/ReklameScript-Regular_DEMO.otf", "r");
	if (!file) {
		Serial.println("file open failed");
	}
	else {
		Serial.println("File successful opened");
	}
*/



temp += "<html>";
	temp += "<head>";
	temp += "<title>ESP8266 Demo</title>";
	temp += "<style>";
		temp += "@font-face {";
			temp += "font-family: Reklame";
			temp += "src: url('/ReklameScript-Regular_DEMO.otf')";
		temp += "}";
		temp += "#logo {font-family: 'Century Gothic'; font-size:300%;}";
		temp += "body { background-color: #000000; font-family: Arial, Helvetica, Sans-Serif; Color: #FFFFFF; }";
		temp += "input[type='text'] { border: 0px;  height: 10%; margin-bottom: 5%}";
		temp += "input[type='submit'] { width: 30%; height: 10%}";
				//Toggle button start
			temp += "\n .switch { position: relative; display: inline-block; width: 60px; height: 34px; }";
			temp += "\n .switch input{ display:none; }";
			temp += "\n .slider{ position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; -webkit-transition: .4s; transition: .4s; }";
			temp += "\n .slider:before{ position: absolute; content: ''; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; -webkit-transition: .4s; transition: .4s; }";
			temp += "\n input:checked + .slider{ background-color: #2196F3; }";
			temp += "\n input:focus + .slider{ box-shadow: 0 0 1px #2196F3; }";
			temp += "\n input:checked + .slider:before{ -webkit-transform: translateX(26px); -ms-transform: translateX(26px); transform: translateX(26px); }";
				//Toggle button end
			
				//Tab view start
			temp += "\n ul.tab{ list-style-type: none; margin: 0; padding: 0; overflow: hidden; border: 1px solid #ccc; background-color: #f1f1f1;}";
			temp += "\n ul.tab li{ float: left; }";
			temp += "\n ul.tab li a{display: inline-block; color: black; text-align: center; padding: 14px 16px; text-decoration: none; transition: 0.3s; font-size: 17px;}";
			temp += "\n ul.tab li a:hover{ background-color: #ddd; }";
			temp += "\n ul.tab li a:focus, .active{ background-color: #ccc;}";
			temp += "\n .tabcontent{ display: none; padding: 6px 12px; border: 1px solid #ccc; border-top: none;}";
			
		temp += "</style>";
	
	temp += "<script type = 'text/javascript' >";
		temp += "window.setInterval(function updateTemp(){";
			temp += "document.getElementById('currentTemp').innerHTML = '"; temp += input; temp += "';";
			temp += "document.getElementById('currentSetpoint').innerHTML = '"; temp += setpoint; temp += "';";
		temp += "}, 1000); ";

		temp += "function openCity(evt, cityName) {";
			temp += "	var i, tabcontent, tablinks;";
			temp += "	tabcontent = document.getElementsByClassName('tabcontent');";
			temp += "	for (i = 0; i < tabcontent.length; i++) {";
			temp += "		tabcontent[i].style.display = 'none';";
			temp += "	}";
			temp += "	tablinks = document.getElementsByClassName('tablinks');";
			temp += "	for (i = 0; i < tablinks.length; i++) {";
			temp += "		tablinks[i].className = tablinks[i].className.replace(' active', '');";
			temp += "	}";
			temp += "	document.getElementById(cityName).style.display = 'block';";
			temp += "	evt.currentTarget.className += ' active';";
			temp += "}";


	temp += "</script>";

	temp += "</head>";
	temp += "<body onload='updateTemp'>";
		temp += "<div id='logo' align='center'>SousVide</div>";
				//Tab view HTML start

				temp += "<ul class = 'tab'>";
				temp += "<li><a href = 'javascript:void(0)' class = 'tablinks' onclick = \"openCity(event,'London')\">London</a></li>";
				temp += "<li><a href = 'javascript:void(0)' class = 'tablinks' onclick = \"openCity(event,'Paris')\">Paris</a></li>";
				temp += "</ul>";

				temp += "<div id = 'London' class = 'tabcontent'>";
				temp += "<h3>London</h3>";
				
								temp += "<div align='center'>";
								temp += "<br>Setpoint: ";
								temp += "<div align='center' id = 'currentSetpoint'>"; temp += "</div>";
								temp += "<br>Temperature: ";
								temp += "<div align='center' id = 'currentTemp'>"; temp += "</div>";
								temp += "</div><br>Heater state: "; temp += stateInfo; temp += "<br>";

				temp += "</div>";

				temp += "<div id = 'Paris' class = 'tabcontent'>";
				temp += "<h3>Paris</h3>";
								
								temp += "<div align='center'>";
								temp += "<form action = 'http://192.168.4.1/submit' method = 'POST'>";
								temp += "	F_name: <input type = 'text' name = 'fname'><br>";
								temp += "	<input type = 'submit' value = 'Send'>";

								temp += "<label class = 'switch'>";
								temp += "<input type = 'checkbox'>";
								temp += "<div class = 'slider'></div>";
								temp += "</label>";
				temp += "</div>";
			temp += "</div>";

				//Tab view HTML end
			
		temp += "</body>";
temp += "</html>";

server.send(200, "text/html", temp);
}
void handleSubmit() {
	String getData = server.arg("fname");

	setpoint = getData.toFloat();
	
}

void handleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for (uint8_t i = 0; i < server.args(); i++) {
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}

	server.send(404, "text/plain", message);
}

void setup(void) {
	delay(1000);
	pinMode(RELAY_PIN, OUTPUT);
	Serial.begin(115200);
	
	sensors.begin();

	WiFi.softAP(ssid, password);
	
	//tell the PID to range between 0 and the full window size
	setpoint = 40.00;
	myPID.SetOutputLimits(0, WindowSize);

	//turn the PID on
	myPID.SetMode(AUTOMATIC);


	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);
	server.on("/submit", handleSubmit);
	server.begin();
	Serial.println("HTTP server started");
	
}

void loop(void) {
	input = ReadTemperature();

	Serial.println("Input: ");
	Serial.print(input);
	Serial.println("Output: ");
	Serial.print(output);
	Serial.println("Setpoint: ");
	Serial.print(setpoint);
	Serial.println("---------");

	myPID.Compute();

	RelayOperation();

	server.handleClient();
}
