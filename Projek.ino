#include <Arduino.h>
#include <HX711.h>
#include <ESP8266WiFi.h>
#include <CTBot.h>

HX711 scale;
CTBot myBot;

#define LOADCELL_DOUT_PIN D2  // GPIO4 pada ESP8266
#define LOADCELL_SCK_PIN D1   // GPIO5 pada ESP8266
const int trigPin = D5;       // GPIO14 pada ESP8266
const int echoPin = D6;       // GPIO12 pada ESP8266
#define SOUND_SPEED 0.034

// Ganti dengan faktor kalibrasi yang Anda dapatkan
float calibration_factor = 55000.0;  // Contoh nilai, ganti dengan nilai kalibrasi Anda

// WiFi credentials
const char* ssid = "Grachia";
const char* password = "yonatanyl1";

// Telegram Bot Token
String token = "7133603663:AAGn8F7EdnY-FIqq8u56hj4tuBn-_by8utc";

// Chat ID
int64_t chat_id = 6726216225;  // Ganti dengan ID obrolan Anda

void setup() {
  Serial.begin(115200);

  // Menghubungkan ke WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  
  Serial.println("Connected to WiFi");

  // Menghubungkan ke Bot Telegram
  myBot.wifiConnect(ssid, password);
  myBot.setTelegramToken(token);
  
  if (myBot.testConnection()) {
    Serial.println("Bot connected successfully");
  } else {
    Serial.println("Bot connection failed");
  }

  // Inisialisasi HX711
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();  // Reset the scale to 0
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.println("Initialized");
}

float readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distanceCm = duration * SOUND_SPEED / 2.0;
  return distanceCm;
}

void loop() {
  // Membaca berat dalam kilogram
  float berat = scale.get_units(10);  

  // Membaca jarak dari sensor ultrasonik
  float jarak = readDistance(); 
  // Menampilkan berat dan jarak di Serial Monitor
  berat = berat;
  Serial.print("Berat: ");
  Serial.print(berat);
  Serial.println("KG");
  Serial.print("Jarak: ");
  Serial.print(jarak);
  Serial.println(" cm");
  
  // Mengirim pesan ke Bot Telegram jika tempat sampah penuh
  if (jarak <= 5 || berat >= 50.0) {  // Kapasitas maksimum 5 kg
    String message = "Tempat Sampah Penuh!\n";
    message += "Berat sampah: " + String(berat) + " KG";
    myBot.sendMessage(chat_id, message);
    Serial.println("Tempat Sampah Penuh! Pesan Terkirim ke Bot Telegram");
  }
  
  delay(1000); // Berikan jeda satu detik
}
