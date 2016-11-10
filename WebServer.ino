

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
File fsUploadFile;
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

void handleRoot() {
	char temporary[2000];
	temp = "";

	temp += "<html>";
	temp += "<head>";
	temp += "<title>ESP8266 Demo</title>";
	temp += "<style>";
	temp += "@font-face {";
	temp += "font-family: Reklame";
	temp += "src: url('/ReklameScript-Regular_DEMO.otf')";
	temp += "}";
	temp += "#logo {font-family: 'Century Gothic'; font-size:300%;}";
	
	temp += ".Row{ display: table; table-layout: fixed; border-spacing: 10px; }";
	temp += ".Column{ display: table-cell;}";
	temp += ".circle{ background-image:linear-gradient(13deg, #3023AE, #C86DD7); border-radius: 100% ; width: 150px; height: 150px; z-index: -1; position:relative;}";
	temp += ".circle2{ background-color: #000; border-radius: 100%; width: 130px; height: 130px; margin: 10px; position: absolute; text-align: center; line-height: 75px;}";
	
	
	temp += "body { background-color: #000000; font-family: Arial, Helvetica, Sans-Serif; Color: #FFFFFF; }";
	temp += "input[type='text'] { border: 0px; margin-bottom: 5%}";
	temp += "input[type='number'] { border: 0px; margin-bottom: 5%; width:10%}";
	temp += "input[type='submit'] { width: 30%; height: 4%}";
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
	temp += "\n ul.tab{ list-style-type: none; margin: 0; width: 50%; padding: 0; overflow: hidden; border: 1px solid #ccc; background-color: #f1f1f1;}";
	temp += "\n ul.tab li{ float: left; }";
	temp += "\n ul.tab li a{display: inline-block; color: black; text-align: center; padding: 14px 16px; text-decoration: none; transition: 0.3s; font-size: 17px;}";
	temp += "\n ul.tab li a:hover{ background-color: #ddd; }";
	temp += "\n ul.tab li a:focus, .active{ background-color: #ccc;}";
	temp += "\n .tabcontent{ display: none; width: 47%; padding: 6px 12px; border: 1px solid #ccc; border-top: none;}";

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
	temp += "<div id='logo' align='center'><img src='logo.jpg'></div>";
	//Tab view HTML start

	temp += "<div align = 'center'>";	//1 - begin
	temp += "<ul class = 'tab'>";
	temp += "<li><a href = 'javascript:void(0)' class = 'tablinks' onclick = \"openCity(event,'Info')\">Info</a></li>";
	temp += "<li><a href = 'javascript:void(0)' class = 'tablinks' onclick = \"openCity(event,'Manual')\">Manual</a></li>";
	temp += "</ul>";

	temp += "<div id = 'Info' class = 'tabcontent'>";	//1.1 - begin
	temp += "<h3>Info</h3>";

	temp += "<div align = 'center' class = 'Row'>";
		temp += "<div  class = 'Column'>";
		temp += "Setpoint:";
			temp += "<div class = 'circle'>";
				temp += "<div id = 'currentSetpoint' class = 'circle2' >";
				temp += setpoint;
				temp += "</div>";
			temp += "</div>";
		temp += "</div>";

		temp += "<div class = 'Column'>";
			temp += "Temperature: ";
			temp += "<div class = 'circle' >";
				temp += "<div id = 'currentTemp' class = 'circle2' >";
				temp += input;
				temp += "</div>";

			temp += "</div>";
		temp += "</div>";
		
		temp += "<div class = 'Column'>";
			temp += "Time remain: ";
				temp += "<div class = 'circle'>";

				temp += "<div id = 'timeRemain' class = 'circle2'>"; 
				temp += countimer.getCurrentTime();
				temp += "</div>";
			temp += "</div>";

		temp += "</div>";
		temp += "</div>Heater state : &#x23FB;";

	temp += "</div>";		//1.1 - end

	temp += "<div id = 'Manual' class = 'tabcontent'>";	//1.2 - begin
	temp += "<h3>Manual</h3>";


	temp += "<div align='center'>";	//1.2.1 - begin
	temp += "<form action = 'http://192.168.4.1/submit' method = 'POST'>";
	temp += "	Setpoint: <input type = 'number' name = 'setpoint' min='0'><br>";
	temp += "	Timer(hh:mm): <input type = 'number' name = 'hours' min='0'>:<input type = 'number' name = 'minutes' min='0'><br>";
	temp += "	<input type = 'submit' value = 'Send'>";

	temp += "</div>";	//1.2.1 - end
	temp += "</div>";			//1.2 - end
	temp += "</div>";				//1 - end
									//Tab view HTML end

	temp += "</body>";
	temp += "</html>";

	server.send(200, "text/html", temp);
}
void handleSubmit() {
	temp = "";

	ChangeHeatingState();

	if (heatingState == true) {
		setpoint = server.arg("setpoint").toFloat();
		hours = server.arg("hours").toInt();
		minutes = server.arg("minutes").toInt();
		countimer.setCounter(hours, minutes, 0, countimer.COUNT_DOWN, ChangeHeatingState);
		countimer.restart();
		countimer.pause();
		restartFlag = true;
	}
	else {
		RelayState(LOW, "off");
		countimer.stop();
	}
	temp += "<html>";
		temp+="<script type = 'text/javascript'>";
			temp += "function RedirectToMainMenu(){";
				temp += "window.location = 'http://192.168.4.1/';";
			temp += "};";
			temp += "</script>";
		temp += "<body onload='RedirectToMainMenu()'>";	
		temp += "</body>";
	temp += "</html>";

	server.send(200, "text/html", temp);
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

String getContentType(String filename) {
	if (server.hasArg("download")) return "application/octet-stream";
	else if (filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".png")) return "image/png";
	else if (filename.endsWith(".gif")) return "image/gif";
	else if (filename.endsWith(".jpg")) return "image/jpeg";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".xml")) return "text/xml";
	else if (filename.endsWith(".pdf")) return "application/x-pdf";
	else if (filename.endsWith(".zip")) return "application/x-zip";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}

bool handleFileRead(String path) {
	Serial.println("handleFileRead: " + path);
	if (path.endsWith("/")) path += "index.htm";
	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
		if (SPIFFS.exists(pathWithGz))
			path += ".gz";
		File file = SPIFFS.open(path, "r");
		size_t sent = server.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}

String formatBytes(size_t bytes) {
	if (bytes < 1024) {
		return String(bytes) + "B";
	}
	else if (bytes < (1024 * 1024)) {
		return String(bytes / 1024.0) + "KB";
	}
	else if (bytes < (1024 * 1024 * 1024)) {
		return String(bytes / 1024.0 / 1024.0) + "MB";
	}
	else {
		return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
	}
}

void setup(void) {
	delay(1000);
	pinMode(RELAY_PIN, OUTPUT);
	Serial.begin(115200);

	sensors.begin();
	sensors.setResolution(9);

	WiFi.softAP(ssid, password);
	
	//tell the PID to range between 0 and the full window size
	setpoint = 40.00;
	input = 0;

	myPID.SetOutputLimits(0, WindowSize);
	//turn the PID on
	myPID.SetMode(AUTOMATIC);

	countimer.setInterval(RefreshClock, 1000);

	SPIFFS.begin();
	{
		Dir dir = SPIFFS.openDir("/");
		while (dir.next()) {
			String fileName = dir.fileName();
			size_t fileSize = dir.fileSize();
			Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
		}
		Serial.printf("\n");
	}

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);
	server.on("/submit", handleSubmit);
	server.on("/logo", HTTP_GET, []() {
		if (!handleFileRead("/logo.jpg")) server.send(404, "text/plain", "FileNotFound");
	});
	server.onNotFound([]() {
		if (!handleFileRead(server.uri()))
			server.send(404, "text/plain", "FileNotFound");
	});
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
