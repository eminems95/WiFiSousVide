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


	temp += "$(function() {";
	temp += "$('form').submit(function() {";
	temp += "$.post('http://192.168.4.1/submit', function() {";
	temp += "window.location = '/';";
	temp += "});";
	temp += "return false;";
	temp += "});";
	temp += "});";


	temp += "</script>";

	temp += "</head>";
	temp += "<body onload='updateTemp'>";
	temp += "<div id='logo' align='center'>SousVide</div>";
	//Tab view HTML start

	temp += "<div align = 'center'>";	//1 - begin
	temp += "<ul class = 'tab'>";
	temp += "<li><a href = 'javascript:void(0)' class = 'tablinks' onclick = \"openCity(event,'Info')\">Info</a></li>";
	temp += "<li><a href = 'javascript:void(0)' class = 'tablinks' onclick = \"openCity(event,'Manual')\">Manual</a></li>";
	temp += "</ul>";

	temp += "<div id = 'Info' class = 'tabcontent'>";	//1.1 - begin
	temp += "<h3>Info</h3>";

	temp += "<div align='center'>";
	temp += "<br>Setpoint: ";
	temp += "<div align='center' id = 'currentSetpoint'>"; temp += "</div>";
	temp += "<br>Temperature: ";
	temp += "<div align='center' id = 'currentTemp'>"; temp += "</div>";
	temp += "</div><br>Heater state: "; temp += stateInfo; temp += "<br>";

	temp += "</div>";		//1.1 - end

	temp += "<div id = 'Manual' class = 'tabcontent'>";	//1.2 - begin
	temp += "<h3>Manual</h3>";


	temp += "<div align='center'>";	//1.2.1 - begin
	temp += "<form action = 'http://192.168.4.1/submit' method = 'POST'>";
	temp += "	Setpoint: <input type = 'number' name = 'setpoint' min='0'><br>";
	temp += "	Timer(hh:mm): <input type = 'number' name = 'hours' min='0'>:<input type = 'number' name = 'minutes' min='0'><br>";
	temp += "	<input type = 'submit' value = 'Send'>";

	/*temp += "<label class = 'switch'>";
	temp += "<input type = 'checkbox'>";
	temp += "<div class = 'slider'></div>";
	temp += "</label>";
	*/
	temp += "</div>";	//1.2.1 - end
	temp += "</div>";			//1.2 - end
	temp += "</div>";				//1 - end
									//Tab view HTML end

	temp += "</body>";
	temp += "</html>";

	server.send(200, "text/html", temp);
}
void handleSubmit() {

	ChangeHeatingState();

	if (heatingState == true) {
		setpoint = server.arg("setpoint").toFloat();
		hours = server.arg("hours").toInt();
		minutes = server.arg("minutes").toInt();
		countimer.setCounter(hours, 0, minutes, countimer.COUNT_DOWN, ChangeHeatingState);
		countimer.restart();
		countimer.pause();
		restartFlag = true;
	}
	else {
		RelayState(LOW, "off");
		countimer.stop();
	}
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