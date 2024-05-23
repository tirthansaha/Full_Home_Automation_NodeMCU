#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

#define BLYNK_TEMPLATE_ID "TMPL6K8dXNgjm"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "preE3cRscviPPvt2NwBuP4R_wONm6um2"

//Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "preE3cRscviPPvt2NwBuP4R_wONm6um2";//Enter your Auth token
char ssid[] = "Morich";//Enter your WIFI name
char pass[] = "amarjhallage";//Enter your WIFI password

DHT dht(D3, DHT11); //(sensor pin,sensor type)
BlynkTimer timer;
bool pirbutton = 0;

// Define component pins
#define Buzzer D0
#define trig D4
#define echo D5
#define PIR D6
#define relay1 D7
#define fire D8 

//Get buttons values
BLYNK_WRITE(V1) {
  pirbutton = param.asInt();
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(Buzzer, OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(fire, INPUT);
  digitalWrite(relay1, LOW);
  lcd.print("Connecting...");
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  lcd.clear();
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("Home Automation");
  lcd.setCursor(0, 1);
  lcd.print("Security System");
  delay(4000);
  lcd.clear();

//Call the functions
  timer.setInterval(100L, DHT11sensor);
  timer.setInterval(100L, pirsensor);
  timer.setInterval(100L, ultrasonic);
  timer.setInterval(100L, firesensor);
}

//Get the DHT11 sensor values
void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V2, h);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(int(t));
  lcd.print("C");

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(int(h));
  lcd.print("%");

}

//Get the PIR sensor values
void pirsensor() {
  bool value = digitalRead(PIR);
  if (pirbutton == 1) {
    if (value == 0) {
      digitalWrite(Buzzer, LOW);
    } else if (value == 1) {
      Blynk.logEvent("theft_alert");
      digitalWrite(Buzzer, HIGH);
    }
  }
}

//Get the ultrasonic sensor values
void ultrasonic() {
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  long cm = t / 29 / 2;
  Blynk.virtualWrite(V4, cm);

  lcd.setCursor(0, 1);
  lcd.print("W:");
  lcd.print(cm);
  lcd.print("  ");
}

//Get buttons values
BLYNK_WRITE(V5) {
 bool RelayOne = param.asInt();
  if (RelayOne == 0) {
    digitalWrite(relay1, LOW);
  } else {
    digitalWrite(relay1, HIGH);
  }
}

//Fire sensor
void firesensor(){
int fire_det = digitalRead(fire);
if (fire_det == 1){
  digitalWrite(Buzzer, LOW);
 }
else if(fire_det == 0){
  digitalWrite(Buzzer, HIGH);
  Blynk.logEvent("fire_alert");
 }
 delay(1000);
}

void loop() {
  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer
}
