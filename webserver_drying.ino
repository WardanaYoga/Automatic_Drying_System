#include <Stepper.h>
#include <WiFi.h>

// Definisi pin
#define raindrop 35     // Pin GPIO35 untuk sensor hujan
#define POWER_PIN 32    // Pin GPIO32 untuk menghidupkan/mematikan sensor hujan
#define ldrpin 33       // Pin GPIO33 untuk sensor cahaya
#define pinled 25       // Pin GPIO25 untuk LED
#define IN1 26          // Pin GPIO26 untuk motor stepper
#define IN2 27          // Pin GPIO27 untuk motor stepper
#define IN3 14          // Pin GPIO14 untuk motor stepper
#define IN4 12          // Pin GPIO12 untuk motor stepper

// WiFi credentials
const char* ssid = "LOGIN WIR";
const char* password = "12345678";

// Inisialisasi server di port 80
WiFiServer server(80);

int lightState;
int prevLightState = -1; // Menyimpan kondisi sebelumnya untuk sensor cahaya
int rainState;
int prevRainState = -1;  // Menyimpan kondisi sebelumnya untuk sensor hujan

Stepper stepper(2048, IN1, IN3, IN2, IN4); // Jumlah langkah: 2048 (motor 28BYJ-48)

// Variabel untuk menyimpan status output
String outputState = "off";

void setup() {
  Serial.begin(115200);
  pinMode(POWER_PIN, OUTPUT);  // Mengatur pin daya sensor hujan sebagai OUTPUT
  pinMode(raindrop, INPUT);    // Mengatur pin sensor hujan sebagai INPUT
  pinMode(ldrpin, INPUT);      // Mengatur pin sensor cahaya sebagai INPUT
  pinMode(pinled, OUTPUT);     // Mengatur pin LED sebagai OUTPUT
  pinMode(IN1, OUTPUT);        // Mengatur pin motor stepper sebagai OUTPUT
  pinMode(IN2, OUTPUT);        // Mengatur pin motor stepper sebagai OUTPUT
  pinMode(IN3, OUTPUT);        // Mengatur pin motor stepper sebagai OUTPUT
  pinMode(IN4, OUTPUT);        // Mengatur pin motor stepper sebagai OUTPUT

  // Menghubungkan ke WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Menampilkan alamat IP
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Mendengarkan client yang masuk

  if (client) {                             // Jika ada client baru yang terhubung
    Serial.println("New Client.");
    String currentLine = "";                // Membuat String untuk menyimpan data dari client
    while (client.connected()) {            // Loop selama client terhubung
      if (client.available()) {             // Jika ada data yang tersedia dari client
        char c = client.read();             // Membaca byte
        Serial.write(c);                    // Menulis byte ke serial
        if (c == '\n') {                    // Jika byte adalah karakter newline
          // Jika currentLine kosong, berarti ada dua newline berturut-turut, yang menandakan akhir request dari client
          if (currentLine.length() == 0) {
            // Mengirim respon HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // HTML untuk menampilkan status sensor dan kontrol
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<p>Rain State: " + String(rainState == LOW ? "Raining" : "Not Raining") + "</p>");
            client.println("<p>Light State: " + String(lightState == LOW ? "Light" : "Dark") + "</p>");
            client.println("<p>LED State: " + outputState + "</p>");
            
            // Display current state, and ON/OFF buttons for LED control  
            if (outputState == "off") {
              client.println("<p><a href=\"/led/on\"><button class=\"button\">Turn LED ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">Turn LED OFF</button></a></p>");
            }
            
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // Memeriksa perintah dari client
        if (currentLine.endsWith("GET /led/on")) {
          digitalWrite(pinled, HIGH);
          outputState = "on";
        } else if (currentLine.endsWith("GET /led/off")) {
          digitalWrite(pinled, LOW);
          outputState = "off";
        }
      }
    }
    // Menutup koneksi
    client.stop();
    Serial.println("Client disconnected.");
  }
  
  lightState = digitalRead(ldrpin);
  rainsensor();

  if (lightState != prevLightState || rainState != prevRainState) {
    handleCases();
    prevLightState = lightState;
    prevRainState = rainState;
  }
}

void rainsensor() {
  digitalWrite(POWER_PIN, HIGH);  // Menghidupkan daya sensor hujan
  delay(10);                      // Menunggu 10 milidetik
  rainState = digitalRead(raindrop); // Membaca kondisi sensor hujan
  digitalWrite(POWER_PIN, LOW);   // Mematikan daya sensor hujan
  delay(3000);                    // Menunggu 3 detik untuk memperpanjang umur sensor
}

void handleCases() {
  if (lightState == LOW && rainState == HIGH) {
    Serial.println("Cuaca Cerah");
    Serial.println("...............");
    digitalWrite(pinled, LOW);
    stepper.setSpeed(10);  // Atur kecepatan motor
    stepper.step(2048);    // Putar 2048 langkah (satu putaran)
    stopMotor();
  } else if (lightState == LOW && rainState == LOW) {
    Serial.println("Terang dan Cuaca Hujan");
    Serial.println("...............");
    digitalWrite(pinled, LOW);
    stepper.setSpeed(10);  // Atur kecepatan motor
    stepper.step(-2048);   // Putar 2048 langkah berlawanan arah jarum jam (satu putaran)
    stopMotor();
  } else if (lightState == HIGH) {
    Serial.println("Gelap");
    Serial.println("...............");
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
