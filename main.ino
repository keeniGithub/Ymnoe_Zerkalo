#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <I2C_graphical_LCD_display.h>
#include "SparkFun_SGP30_Arduino_Library.h" 


I2C_graphical_LCD_display lcd;
Adafruit_BME280 bme280;
SGP30 mySensor;
int sc, mn, hr;
int console;
int taimerSec = 0, taimerMin = 0, taimerHour = 0;
int flag;
int i = 1; 
int a = 1;  

const char* ssid = "kivi";
const char* password = "12301230";


String openWeatherMapApiKey = "279385a3bb0de1fc7c281518e98969c3";
String city = "Saint Petersburg";
String countryCode = "RU";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

String jsonBuffer;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  WiFi.begin(ssid, password);
  bme280.begin();
 
  if (mySensor.begin() == false) {
    delay(1000);
    lcd.clear(1,25,128,25);
    Serial.println("No SGP30 Detected. Check connections.");  
    lcd.gotoxy (40, 25);
    lcd.string("SPG30 no detected. Check the sensor", false);
    while (1);
  }
  mySensor.initAirQuality();
  lcd.begin(); 

  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.clear(1,25,128,25);
    lcd.gotoxy (1, 25);
    lcd.string("Wi-Fi not connected", false);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
  lcd.clear(1,25,128,25);
}

void chasi()
{
  float t = bme280.readTemperature();
  char temper[10];
  char hrChar[10];
  char mnChar[10];
  char scChar[10];
  itoa(t, temper, 10);
  itoa(hr, hrChar, 10);
  itoa(mn, mnChar, 10);
  itoa(sc, scChar, 10);
  lcd.gotoxy (20, 10);
  lcd.string("temper: ", false);
  lcd.string(temper, false);
  lcd.string(" | ", false);
  lcd.gotoxy (20, 30);

  if (hr <= 9) 
  {
    lcd.string("0", false);
  }

  lcd.string(hrChar, false); 
  lcd.string(":", false);
  lcd.gotoxy (40, 30);

  if (mn <= 9) 
  {
    lcd.string("0", false);
  }

  lcd.string(mnChar, false);
  lcd.string(":", false);
  lcd.gotoxy (60, 30);

  if (sc <= 9) 
  {
    lcd.string("0", false);
  }

  lcd.string(scChar, false);
  sc++; 

  if (sc == 60) 
  {
    sc = 0;
    mn = mn + 1;
  }

  if (mn == 60)
  {
    mn = 0;
    hr = hr + 1;
  }

  if (hr == 24)
  {
    hr = 0; 
  }

  delay(1000);  
}


void chasiSetting()
{
  lcd.gotoxy (20, 30);
  lcd.string("Vvedite secyndi: ", false);
  delay(2000);
  sc = Serial.parseInt();
  if(a == 1)
  {
    lcd.gotoxy (20, 30);
    lcd.string("Vvedite minyti: ", false);
    delay(2000);
    mn = Serial.parseInt();
    a = 2;
  }
  if(a == 2){
    lcd.gotoxy (20, 30);
    lcd.string("Vvedite chasi: ", false);
    delay(2000);
    hr = Serial.parseInt();
    a = 3;
    lcd.clear(10,30,128,30);
  }
  if(hr >= 24){
    lcd.gotoxy (10, 30);
    lcd.string("Chasi dolzi bit <24!", false);
    delay(1200);
    lcd.clear(10,30,128,30);
  }
}

void taimerr()
{
  lcd.gotoxy (20, 45);
  lcd.string("Vvedite secyndi: ", false);
  delay(2000);
  sc = sc+2;
  taimerSec = Serial.parseInt();
  if(i == 1)
  {
    lcd.gotoxy (20, 45);
    lcd.string("Vvedite minyti: ", false);
    delay(2000);
    sc = sc+2;
    taimerMin = Serial.parseInt();
    i = 2;
  }
  if(i == 2)
  {
    while(taimerSec != 0 or taimerMin != 0 )
    {
      lcd.clear(0, 45, 128, 45);
      //Co2();
      char minChar[10];
      char SecChar[10];
      itoa(taimerSec, SecChar, 10);
      itoa(taimerMin, minChar, 10);
      
      if(taimerMin >= 1 and taimerSec == 0)
      {
        taimerMin -= 1;
        taimerSec += 60;
      }

      lcd.gotoxy(20, 45);
      
      if(taimerMin <= 9)
      {
        lcd.string("0", false);
      }
      lcd.string(minChar, false);
      lcd.string(":", false);
      lcd.gotoxy (40, 45);
      
      if(taimerSec <=9)
      {
        lcd.string("0", false);
      }
      
      lcd.string(SecChar, false);
      chasi();
      lcd.clear(0, 45, 128, 45);
      taimerSec = taimerSec-1;

      if(taimerSec == 0 and taimerMin == 0)
      {
       lcd.gotoxy (20, 45);
       lcd.string("taimer Okonchen", false);
       delay(1000);
       taimerMin = 0; taimerSec = 0;
       lcd.clear(0, 45, 128, 45);
      }
    }
  }
}

void Co2()
{
  mySensor.measureAirQuality();
  int co1 = mySensor.CO2;
  char co2[10];
  itoa(co1, co2, 10);
  lcd.gotoxy (20, 60);
  lcd.string("Co2: ", false);
  lcd.string(co2, false);
}

void debug_mode()
{
  // while(1){
  // lcd.clear(1,1,128,63);
  // delay(1000);
  // lcd.gotoxy(10,10);
  // char ip[64] = WiFi.localIP();
  // lcd.string(ip);
  //}
}

void loop() 
{
console = Serial.read();

chasi();
Co2();

lcd.gotoxy(20, 45);
lcd.string("taimer ne zapyshen", false);

if(console == 'R'){chasiSetting();}
if(console == 'T'){taimerr();}
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
      char street_temper[20];
      int street_temper_orig = 1435;
      itoa(street_temper_orig, street_temper, 20);
      Serial.println(street_temper);
      lcd.gotoxy(70, 10);
      lcd.string(street_temper, false);
        
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    return payload;
}