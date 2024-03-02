#include<Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing reset pin)


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

int minVal = 265;
int maxVal = 402;

double x;
double y;
double z;
char sleep_status = 0;
#define BOTtoken "7017590405:AAEQnfBnxY5MKKCY5s-GlednQi1XfTeiNtY"
#define CHAT_ID "-4173555271" //-1001573046381
#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  pinMode(17, OUTPUT);
  Serial.begin(9600);
#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
#endif
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2);
  display.clearDisplay();

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 5);
  display.print("Soheb");
  display.display();
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect("set up car"); // anonymous ap
  //res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }

}
void loop() {

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  int xAng = map(AcX, minVal, maxVal, -90, 90);
  int yAng = map(AcY, minVal, maxVal, -90, 90);
  int zAng = map(AcZ, minVal, maxVal, -90, 90);

  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

  Serial.print("AngleX= ");
  Serial.println(x);

  Serial.print("AngleY= ");
  Serial.println(y);

  Serial.print("AngleZ= ");
  Serial.println(z);
  Serial.println("-----------------------------------------");

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("Y: ");
  display.println(y);

  if (y > 50) {
    Serial.println("trigger");
    digitalWrite(17, HIGH);
     
        bot.sendMessage(CHAT_ID, "Accident ditected and your car rollover!", "");
      
  }
   if (digitalRead(16) == HIGH) {
       bot.sendMessage(CHAT_ID, "Your car got hit", "");
      }
      
  if (Serial.available()) {
    char sleep_status = Serial.read();
    if (sleep_status == 'a') {

      bot.sendMessage(CHAT_ID, "Your driver feeling sleepy!", "");
      
      if (digitalRead(17) == HIGH) {
        digitalWrite(17, LOW);
      } else {
        digitalWrite(17, HIGH);

      }
    }
  }


  display.display();



  delay(1000);
}
