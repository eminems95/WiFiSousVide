// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

/*#include <DHT.h>
#define DHTPIN 4     // what pin we're connected to
#define RELAY_PIN 5

#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
	Serial.begin(115200);
	Serial.println("DHT11 test!");
	pinMode(RELAY_PIN, OUTPUT);
	dht.begin();
}

void loop() {
	// Wait a few seconds between measurements.
	delay(5000);

	
	float t = dht.readTemperature();

	// Check if any reads failed and exit early (to try again).
	if ( isnan(t) ) {
		Serial.println("Failed to read from DHT sensor!");
		return;
	}

	if (t < 24.00) {
		digitalWrite(RELAY_PIN, HIGH);
	}
	else {
		digitalWrite(RELAY_PIN, LOW);
	}
	// Compute heat index in Fahrenheit (the default)
	//float hif = dht.computeHeatIndex(f, h);
	// Compute heat index in Celsius (isFahreheit = false)
	//float hic = dht.computeHeatIndex(t, h, false);


	Serial.print("Temperature: ");
	Serial.print(t);
	Serial.print(" *C ");
	
}