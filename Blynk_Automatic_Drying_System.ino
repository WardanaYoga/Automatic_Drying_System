#define BLYNK_TEMPLATE_ID "TMPL6xuwiZrIO"
#define BLYNK_TEMPLATE_NAME "Penjemuran"
#define BLYNK_AUTH_TOKEN "47gM8mVxVOLrVo3dYsB7ybkiowMmYXz2" // Ganti dengan Auth Token dari Blynk

#include <BlynkSimpleEsp32.h>
#include <Stepper.h>
#include <WiFi.h>

#define raindrop 35     // ESP32's pin GPIO35 connected to DO pin of the rain sensor
#define POWER_PIN 32    // ESP32's pin GPIO32 that provides the power to the rain sensor
#define ldrpin 33       // ESP32's pin GPIO33 connected to DO pin of the LDR module
#define pinled 25
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 12

char ssid[] = "LOGIN WIR";       // Ganti dengan nama jaringan WiFi Anda
char pass[] = "12345678";        // Ganti dengan password WiFi Anda

int lightState;
int prevLightState = -1; // Menyimpan kondisi sebelumnya untuk sensor cahaya
int rainState;
int prevRainState = -1; // Menyimpan kondisi sebelumnya untuk sensor hujan
int hold = 0;
Stepper stepper(2048, IN1, IN3, IN2, IN4); // Jumlah langkah: 2048 (motor 28BYJ-48)
BlynkTimer Time;
void setup() {
  Serial.begin(115200);
  pinMode(POWER_PIN, OUTPUT);  // configure the power pin as an OUTPUT
  pinMode(raindrop, INPUT);
  pinMode(ldrpin, INPUT);
  pinMode(pinled, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Menghubungkan ke WiFi dan Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Time.setInterval(1000L, sensor);
}
void sensor(){
  lightState = digitalRead(ldrpin);
  rainsensor();

  if (lightState != prevLightState || rainState != prevRainState) {
    handleCases();
    prevLightState = lightState;
    prevRainState = rainState;
  } 
  
  // Kirim data sensor ke Blynk
  Blynk.virtualWrite(V1, lightState); // Kirim status sensor cahaya ke Virtual Pin V1
  Blynk.virtualWrite(V0, rainState);  // Kirim status sensor hujan ke Virtual Pin V2
}

void loop() {
  Blynk.run();
  Time.run();  // Menjalankan Blynk

}

void rainsensor() {
  digitalWrite(POWER_PIN, HIGH);  // turn the rain sensor's power ON
  delay(10);                      // wait 10 milliseconds
  rainState = digitalRead(raindrop);
  digitalWrite(POWER_PIN, LOW);   // turn the rain sensor's power OFF
  delay(3000);                    // pause for 3 sec to avoid reading sensors frequently to prolong the sensor lifetime
}

void handleCases() {
  if (lightState == LOW && rainState == HIGH) {
    Serial.println("Cuaca Cerah");
    Serial.println("...............");
    Blynk.logEvent("cerah","CERAH CUY");
    digitalWrite(pinled, LOW);
    hold = 0;
    stepper.setSpeed(10);  // Atur kecepatan motor
    stepper.step(2048);    // Putar 2048 langkah (satu putaran)
    stopMotor();
  } else if (lightState == LOW && rainState == LOW) {
    Serial.println("Terang dan Cuaca Hujan");
    Serial.println("...............");
    Blynk.logEvent("rainalert","HUJAN CUY");
    digitalWrite(pinled, LOW);
    stepper.setSpeed(10);  // Atur kecepatan motor
    stepper.step(-2048);   // Putar 2048 langkah berlawanan arah jarum jam (satu putaran)
    stopMotor();
  } else if (lightState == HIGH) {
    Serial.println("Gelap");
    Serial.println("...............");
    Blynk.logEvent("gelap","Gelap Euy");
    digitalWrite(pinled, HIGH);
    stepper.setSpeed(10);  // Atur kecepatan motor
    stepper.step(-2048);   // Putar 2048 langkah berlawanan arah jarum jam (satu putaran)
    stopMotor();
  }
}

// Fungsi untuk memberhentikan motor stepper
void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
