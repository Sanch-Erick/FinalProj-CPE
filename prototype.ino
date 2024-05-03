#include <LiquidCrystal.h>
#include <Stepper.h>
#include <dht.h>

dht DHT;
#define DHT11_PIN 13
//water sens
#define POWER_PIN 40
#define SIGNAL_PIN A5

//DC motor pins
int enA = 52;
int in1 = 50;
int in2 = 48;

//water snes
int value = 0;


// LCD pins <--> Arduino pins/ stepper
const int stepsPerRevolution = 2500;
const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;

Stepper myStepper = Stepper(stepsPerRevolution, 26, 28, 30, 32);
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void setup()
{
lcd.begin(8, 2); // set up number of columns and rows
lcd.setCursor(0, 0); // move cursor to (0, 0)
lcd.print("Hello"); // print message at (0, 0)
lcd.setCursor(2, 1); // move cursor to (2, 1)
lcd.print("RIC"); // prpint message at (2, 1)

//DC motor setup
  pinMode(enA, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);

  Serial.begin(9600);
//water
 pinMode(POWER_PIN, OUTPUT); // configure D7 pin as an OUTPUT  
 digitalWrite (POWER_PIN, LOW);
}

void loop()
{
 digitalWrite (POWER_PIN, HIGH); // turn the sensor ON
 delay(10); // wait 10 milliseconds
 value = analogRead (SIGNAL_PIN); // read the analog value from sensor
 digitalWrite (POWER_PIN, LOW); // turn the sensor OFF
 Serial.print("water value: " );
 Serial.println(value);

myStepper.setSpeed(10);
myStepper.step(stepsPerRevolution);
directionControl(); 

int chk = DHT.read11(DHT11_PIN);
 Serial.print("Temperature = ");
 Serial.println(DHT.temperature);
 Serial.print("Humidity = ");
 Serial.println(DHT.humidity);
delay(1000);

}

void directionControl() {
  analogWrite(enA, 255);
  digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	delay(2000);
  digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
}