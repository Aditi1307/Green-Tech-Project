#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "Sandesh"; // Your Network SSID
const char* password = "1587394w"; // Your Network Password

// ThingSpeak credentials
unsigned long myChannelNumber = 2613818; // Your Channel Number
const char* myWriteAPIKey = "M6U4UJ6IBJA8K8ZF"; // Your Write API Key

// Pin definitions
const int SOIL_MOISTURE_SENSOR = A0;
const int VIBRATION_SENSOR = 5;
#define DHTPIN 2
#define DHTTYPE DHT11
const int TRIG_PIN = 12; // Ultrasonic sensor TRIG pin
const int ECHO_PIN = 13; // Ultrasonic sensor ECHO pin

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// WiFi client
WiFiClient client;

// Function to initialize vibration sensor
long TP_init() {
  delay(10);
  long measurement = pulseIn(VIBRATION_SENSOR, HIGH); // wait for the pin to get HIGH and returns measurement
  return measurement;
}

// Function to get distance from ultrasonic sensor
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2; // Calculate the distance in cm
  return distance;
}

void setup() {
  Serial.begin(9600);
  pinMode(VIBRATION_SENSOR, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Start the DHT sensor
  dht.begin();
}

void loop() {
  // Read soil moisture sensor
  int soilMoistureValue = analogRead(SOIL_MOISTURE_SENSOR);
  Serial.print("Soil Moisture = ");
  Serial.println(soilMoistureValue);

  // Read vibration sensor
  long measurement = TP_init();
  delay(10);

  Serial.print("Vibration measurement = ");
  Serial.println(measurement);

  // Read DHT sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");

  // Read ultrasonic sensor
  long distance = getDistance();
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");

  // Upload data to ThingSpeak
  ThingSpeak.setField(1, soilMoistureValue);
  ThingSpeak.setField(2, measurement);
  ThingSpeak.setField(3, temperature);
  ThingSpeak.setField(4, humidity);
  ThingSpeak.setField(5, distance);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Wait a few seconds before the next loop
  delay(2000);
}
