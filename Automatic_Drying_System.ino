
#define POWER_PIN 32  // ESP32's pin GPIO32 that provides the power to the rain sensor
#define raindrop 13     // ESP32's pin GPIO13 connected to DO pin of the rain sensor
#define ldrpin 12  // ESP32's pin GPIO13 connected to DO pin of the LDR module
#define pinled 4

int lightState;
int rain_state;

void setup() {
  // initialize serial communication
  Serial.begin(115200);
  // initialize the Arduino's pin as an input
  pinMode(POWER_PIN, OUTPUT);  // configure the power pin pin as an OUTPUT
  pinMode(raindrop, INPUT);
  pinMode(ldrpin, INPUT);
  pinMode(pinled, OUTPUT);
}

void loop() {
  lightState= digitalRead(ldrpin);
  digitalWrite(POWER_PIN, HIGH);  // turn the rain sensor's power  ON
  delay(10);                      // wait 10 milliseconds
  rain_state = digitalRead(raindrop);
  digitalWrite(POWER_PIN, LOW);  // turn the rain sensor's power OFF

  /** if (rain_state == HIGH){
    Serial.println("The rain is NOT detected");
    Serial.println("=======");
  }else{
    Serial.println("The rain is detected");
    Serial.println("=======");
  }
  **/
  kasus_1();
  kasus_2();
  kasus_3();
  kasus_4();
  delay(5000);  // pause for 1 sec to avoid reading sensors frequently to prolong the sensor lifetime
}

void kasus_1(){
  if (lightState == LOW && rain_state == HIGH)
  {
    Serial.println("Cuaca Cerah");
    Serial.println("...............");
    digitalWrite(pinled, LOW);
    //kode motor...
    delay(5000);
  }
}
void kasus_2(){
  if (lightState == LOW && rain_state == LOW)
  {
    Serial.println("Terang dan Cuaca Hujan");
    Serial.println("...............");
    digitalWrite(pinled, LOW);
    delay(5000);
  }
}
void kasus_3(){
  if (lightState == HIGH && rain_state == HIGH)
  {
    Serial.println("Gelap dan Tidak Hujan");
    Serial.println("...............");
    digitalWrite(pinled, HIGH);
    delay(5000);
  }
}
void kasus_4(){
  if (lightState == HIGH && rain_state == LOW)
  {
    Serial.println("Gelap dan Cuaca Hujan");
    Serial.println("...............");
    digitalWrite(pinled, HIGH);
    delay(5000);
  }
}