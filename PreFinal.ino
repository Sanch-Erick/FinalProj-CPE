#include <LiquidCrystal.h>
#include <Stepper.h>
#include <dht.h>

//RTC clock
#include <RTClib.h>
//32 potential stepper

RTC_DS1307 rtc;
//53,51,49,47 ligts 
//
//start stop reset
//35    37   39 buttons
int buttonState1 = 1; 
int buttonState2 = 1;
int buttonState3 = 1;

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

//stepper
int previous = 0;

// LCD pins <--> Arduino pins/ stepper
const int stepsPerRevolution = 2500;
const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;

//stepper
Stepper myStepper = Stepper(stepsPerRevolution, 32, 28, 30, 26);
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);


//========================================================================

void setup() {
  Serial.begin(9600);

  //lil intro screen
  lcd.begin(8, 2); // set up number of columns and rows
  lcd.setCursor(0, 0); // move cursor to (0, 0)
  lcd.print("Hello"); // print message at (0, 0)
  lcd.setCursor(2, 1); // move cursor to (2, 1)
  lcd.print("RIC"); // prpint message at (2, 1)

  //motor
  pinMode(enA, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);

  //water
  pinMode(POWER_PIN, OUTPUT); // configure D7 pin as an OUTPUT  
  digitalWrite (POWER_PIN, LOW);

  //  time
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  //LED 
  pinMode(53, OUTPUT);
  pinMode(51, OUTPUT);
  pinMode(49, OUTPUT);
  pinMode(47, OUTPUT);
  //button
  pinMode(35, INPUT);
  pinMode(37, INPUT);
  pinMode(39, INPUT);
  //steppr 
  myStepper.setSpeed(10);
  LCDupdate();
}
bool emergenC() {
      digitalWrite (POWER_PIN, HIGH); // turn the sensor ON
      delay(10); // wait 10 milliseconds
      value = analogRead (SIGNAL_PIN);

      if (value <= 0){
      digitalWrite (POWER_PIN, LOW); // turn the sensor OFF
      return true;
      }else
      {
      return false;
      }
}

void controlVent(Stepper& myStepper) {
  int val = analogRead(A4);
  bool check = false;
  //Serial.print(val);
 // Serial.println(' ');

  if(val <= 341 ){
    while (val < 341 )
    {
      myStepper.step(200);
      val = analogRead(A4);
      check = emergenC();
      if(check != false);
         {
        break;
      }
    }
  } else if(val >=682 ){
    while (val > 682)
    {
      myStepper.step(-200);
      val = analogRead(A4);
      check = emergenC();
      if(check != false);
      {
        break;
      }
    }
  }
}

void controlVent2(Stepper& myStepper) {
  int val = analogRead(A4);
  bool check = false;
  //Serial.print(val);
 // Serial.println(' ');

  if(val <= 341 ){
    while (val < 341 )
    {
      myStepper.step(10);
      val = analogRead(A4);
      
    }
  } else if(val >=682 ){
    while (val > 682)
    {
      myStepper.step(-10);
      val = analogRead(A4);
      
    }
  }
}
//THREE BUTTONS , START STOP RESET?
enum State {
  DISABLED,
  IDLE,
  ERROR,
  RUNNING
};

//pin 18 interupt
State state = DISABLED;

// Function to print the current date and time
void printDateTime() {
  DateTime now = rtc.now();

  Serial.print("Date & Time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
}
void fan() {
  analogWrite(enA, 255);
  digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	
}

void LCDupdate() {
    

    
    int chk = DHT.read11(DHT11_PIN);

lcd.setCursor(0, 1); 
lcd.print("T ");
lcd.print(DHT.temperature);
lcd.print(" H ");
lcd.print(DHT.humidity); 
}

int period =60000;
unsigned long time_now = 0;

void loop() {

    if(millis() >= time_now + period){
        time_now += period;
          LCDupdate();
//Serial.print("TEST: ");
    }
  
  // printDateTime(); 
   // Call the function to print the date and time
  buttonState1 = digitalRead(35);
  buttonState2 = digitalRead(37);
  buttonState3 = digitalRead(39);
 delay(100);
  
  switch (state) {
    case DISABLED:
    {

        lcd.setCursor(0, 0); 
        lcd.print("DISABLED");
      // no checking sensors, ISR start button, Yellow LED
        digitalWrite(49, HIGH);
        controlVent2(myStepper);

        

        //=======================================================
        if (buttonState1 == LOW) { //do ISR eventually
        //Serial.println("Button pressed");
          digitalWrite(49, LOW);
          printDateTime(); 
          
          digitalWrite (POWER_PIN, HIGH); // turn the sensor ON
          delay(10); // wait 10 milliseconds
          value = analogRead (SIGNAL_PIN); // read the analog value from sensor
          digitalWrite (POWER_PIN, LOW); // turn the sensor OFF
          Serial.print("water value: " );
          Serial.println(value);

          int chk = DHT.read11(DHT11_PIN);
          Serial.print("Temperature = ");
          Serial.println(DHT.temperature);
          delay(100);
//==========================================================================
          
          
          if(DHT.temperature <= 24 && value >= 1){
            state = IDLE;
          }else if(DHT.temperature <= 24 && value <= 0){
            state = ERROR;
          }else if(DHT.temperature >= 24 &&value >= 1){
            state = RUNNING;
          }
          break;
        }
        
    }
        break;
    case IDLE:
    {
        controlVent(myStepper);
         
        lcd.setCursor(0, 0); 
        lcd.print("IDLE    "); 
      // Checking water > 100, Green LED
      digitalWrite(53, HIGH);
      digitalWrite (POWER_PIN, HIGH); // turn the sensor ON
      delay(10); // wait 10 milliseconds
      value = analogRead (SIGNAL_PIN);
      digitalWrite (POWER_PIN, LOW);

       if (buttonState2 == LOW)
            {
                printDateTime();
                state = DISABLED;
                digitalWrite(53, LOW);
            }

      if(value <= 1){
            printDateTime(); 
            state = ERROR;
            digitalWrite(53, LOW);
          }
          else if(DHT.temperature >= 24){
            printDateTime(); 
            state = RUNNING;
            digitalWrite(53, LOW);
          }
    }
      break;
    case ERROR:
    {
      // Motor not on at all, Red LED, LCD display error, RESET BUTTON => IDLE iff water >=100
        digitalWrite(51, HIGH);
        lcd.setCursor(0, 0); 
        lcd.print("ERROR   "); 
        if (buttonState2 == LOW)
            {
               printDateTime();
                state = DISABLED;
                digitalWrite(51, LOW);
            }
        if (buttonState3 == LOW)
            {
          digitalWrite (POWER_PIN, HIGH); // turn the sensor ON
          delay(10); // wait 10 milliseconds
          value = analogRead (SIGNAL_PIN); // read the analog value from sensor
          digitalWrite (POWER_PIN, LOW); // turn the sensor OFF

            if(value >= 1){
             printDateTime(); 
             state = IDLE;
             digitalWrite(53, LOW);
             digitalWrite(51, LOW);
          }
            }

    }
      break;
    case RUNNING:
    {
        lcd.setCursor(0, 0); 
        lcd.print("Running ");

       controlVent(myStepper);
      //Motor on, Blue LED, water < 100 => ERROR, Temp < 22
        digitalWrite(47, HIGH);


        if (buttonState2 == LOW)
            {
               printDateTime();
                state = DISABLED;
                digitalWrite(47, LOW);
                digitalWrite(in1, LOW);
            delay(10);
            break;

            }



      
      digitalWrite (POWER_PIN, HIGH); // turn the sensor ON
      delay(10); // wait 10 milliseconds
      value = analogRead (SIGNAL_PIN);
      digitalWrite (POWER_PIN, LOW);

        fan();

       if(value <= 1){
            printDateTime(); 
            state = ERROR;
            digitalWrite(47, LOW);
            digitalWrite(in1, LOW);
            delay(10);
          }
          else if(DHT.temperature <= 21){
            printDateTime(); 
            state = IDLE;
            digitalWrite(47, LOW);
            digitalWrite(in1, LOW);
            delay(10);
          }


    }
      break;
  }
  
  delay(1000);
}