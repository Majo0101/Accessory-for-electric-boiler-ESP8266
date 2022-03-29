#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
#include <OneWire.h> 
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DallasTemperature.h>

#define vystup1 D5
#define vystup2 D6
#define vstup1 D3
#define vstup2 D4
#define ONE_WIRE_BUS D7
#define tuc 1

const char *ssid     = "********";
const char *password = "********";
int teplota1;
int teplota2;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1 = {0x28, 0xFF, 0x64, 0x1E, 0x84, 0x73, 0x32, 0x76};  // 2m cidlo
DeviceAddress sensor2 = {0x28, 0x96, 0x29, 0x96, 0xF0, 0x01, 0x3C, 0xAA}; // 1m cidlo

LiquidCrystal_I2C lcd(0x27, 20, 4);     // LCD 20/4 , adress i2c 0x27

int in1 = 0;
int in2 = 0;

byte internet[] = {
  B10101,
  B01110,
  B00100,
  B00111,
  B00100,
  B00110,
  B00100,
  B00111
};

byte stupen[] = {
  B01000,
  B10100,
  B01000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte ohen[] = {
  B01000,
  B00100,
  B00110,
  B01011,
  B10001,
  B10101,
  B10101,
  B01110
};

byte ventil[] = {
  B11111,
  B00100,
  B00100,
  B01110,
  B10001,
  B10001,
  B01110,
  B00000
};

void teplota()
{
  sensors.requestTemperatures();
  teplota1 = (sensors.getTempC(sensor1));
  teplota2 = (sensors.getTempC(sensor2));
  delay(2000);
}

void setup() {

pinMode(vystup1, OUTPUT);
pinMode(vystup2, OUTPUT);
pinMode(vstup1, INPUT);
pinMode(vstup2, INPUT);
pinMode(tuc, OUTPUT);
//pinMode(LED, OUTPUT);

lcd.init();
lcd.backlight();
lcd.clear();

lcd.createChar(0 , stupen);
lcd.createChar(1 , internet);
lcd.createChar(2 , ohen);
lcd.createChar(3 , ventil);

WiFi.begin(ssid, password);

timeClient.begin();
timeClient.setTimeOffset(3600); // GMT+1

sensors.begin();
sensors.setResolution(sensor1, 9);
sensors.setResolution(sensor2, 9);

for (int reklama = 0; reklama < 2; reklama++)    // version
  {
    lcd.setCursor(6, 1);
    lcd.print ("M4j0 1.0");
    delay(1500);
    lcd.clear ();
  }

}

void loop() {
timeClient.update();

digitalWrite(tuc, LOW);

if(WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(14,3);
    lcd.print(" ");  
  }
else{
  lcd.setCursor(14,3);
  lcd.write(1);
}

int Hodina = timeClient.getHours();
int Minuta = timeClient.getMinutes();

lcd.setCursor(17,3);
lcd.print(":");

if(Hodina > 9)
{
  lcd.setCursor(15,3);
  lcd.print(Hodina);
}
else
{
  lcd.setCursor(15,3);
  lcd.print("0"); 
  lcd.setCursor(16,3);
  lcd.print(Hodina);
}


if(Minuta > 9 )
{
  lcd.setCursor(18,3);
  lcd.print(Minuta);  
}
else
{
  lcd.setCursor(18,3);
  lcd.print("0");
  lcd.setCursor(19,3);
  lcd.print(Minuta);
} 

teplota();


lcd.setCursor(0,0);
lcd.print("Voda:");

lcd.setCursor(0,3);
lcd.print("Kotol:");

lcd.setCursor(10,3);
lcd.write(0);

lcd.setCursor(9,0);
lcd.write(0);

lcd.setCursor(0,1);
lcd.print("Ohrev-");

lcd.setCursor(16,0);
lcd.write(3);




in1 = digitalRead(vstup1);
in2 = digitalRead(vstup2);

if(teplota1 < 0)
{
  lcd.setCursor(5,0);
  lcd.print(" ERR");
}
else
{
  lcd.setCursor(5,0);
  lcd.print(teplota1);
  lcd.setCursor(7,0);
  lcd.print("  ");
}


if(teplota2 < 0)
{
  lcd.setCursor(6,3);
  lcd.print(" ERR");
}
else
{
  lcd.setCursor(6,3);
  lcd.print(teplota2);
  lcd.setCursor(8,3);
  lcd.print("  ");
}

//...................Boil on off..........................
if(in1 == 0)
{
  lcd.setCursor(11,3);
  lcd.write(2);
}
else
{
  lcd.setCursor(11,3);
  lcd.print(" ");
}

//.........................................................

//......................Run.........................

if(in1 == 0 && in2 == 1)
{
  lcd.setCursor(7,1);
  lcd.print("Kotol        ");
  digitalWrite(vystup2, LOW);

  if((teplota1 +3) < teplota2)
  {
    digitalWrite(vystup1, HIGH);
    lcd.setCursor(17,0);
    lcd.print("Zap");
  }
  else
  {
    digitalWrite(vystup1, LOW);
    lcd.setCursor(17,0);
    lcd.print("Vyp");
  } 
  lcd.setCursor(0,2);
  lcd.print("           ");
}

if(in1 == 0 && in2 == 0)
{
  lcd.setCursor(7,1);
  lcd.print("Kotol+Elektro");

  if((teplota1 +3) < teplota2)
  {
    digitalWrite(vystup1, HIGH);
    lcd.setCursor(17,0);
    lcd.print("Zap");

  }
  else
  {
    digitalWrite(vystup1, LOW);
    lcd.setCursor(17,0);
    lcd.print("Vyp");
  } 

  if(5 < Hodina && Hodina < 8 || 11 < Hodina && Hodina < 14)
  {
    digitalWrite(vystup2, HIGH);
    lcd.setCursor(0,2);
    lcd.print("Zasuvka Zap");
  }
  else
  {
    digitalWrite(vystup2, LOW);
    lcd.setCursor(0,2);
    lcd.print("Zasuvka Vyp");
  }

}

if(in1 == 1 && in2 == 0)
{
  lcd.setCursor(7,1);
  lcd.print("Elektro      ");

  if(5 < Hodina && Hodina < 8 || 11 < Hodina && Hodina < 14)
  {
    digitalWrite(vystup2, HIGH);
    lcd.setCursor(0,2);
    lcd.print("Zasuvka Zap");
  }
  else
  {
    digitalWrite(vystup2, LOW);
    lcd.setCursor(0,2);
    lcd.print("Zasuvka Vyp");
  }

    lcd.setCursor(17,0);
    lcd.print("Vyp");
}

if(in1 == 1 && in2 == 1)
{
  lcd.setCursor(7,1);
  lcd.print("Vypnute      ");
  
  digitalWrite(vystup1, LOW);
  digitalWrite(vystup2, LOW);

  lcd.setCursor(17,0);
  lcd.print("Vyp");
  lcd.setCursor(0,2);
  lcd.print("           ");
}
//..................................................................

//if(Hodina == 23)
//{
//  ESP.restart();
//}

}
