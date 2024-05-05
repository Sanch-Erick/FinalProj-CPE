//Created/ compiled by Erick


#include <LiquidCrystal.h>
#include <Stepper.h>
#include <dht.h>


//ADC
#define RDA 0x80
#define TBE 0x20


volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
 
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

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

//DC motor pins 52 50 48

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

//scary stuff, timer
volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned int  *myTCNT1  = (unsigned  int *) 0x84;
volatile unsigned char *myTIFR1 =  (unsigned char *) 0x36;

void myDelay(unsigned long milliseconds) {
    // Calculate the timer count for the delay time
    unsigned long ticks = (F_CPU / 64) * milliseconds / 1000;

    // Ensure ticks does not exceed the maximum value for a 16-bit register
    if (ticks > 65535) {
        ticks = 65535;
    }

    // Stop the timer and clear the settings
    *myTCCR1B &= 0xF8; // 0b 0000 0000

    // Set the counts
    *myTCNT1 = (unsigned int) (65536 - ticks);

    // Start the timer
    *myTCCR1A = 0x0;
    *myTCCR1B |= 0b00000101;

    // Wait for overflow
    while ((*myTIFR1 & 0x01) == 0); // 0b 0000 0000

    // Stop the timer
    *myTCCR1B &= 0xF8; // 0b 0000 0000

    // Reset TOV
    *myTIFR1 |= 0x01;
}

// Define the registers



// Define the registers for pins 52, 50, 48
volatile unsigned char* port_b = (unsigned char*) 0x25; // PB1, PB3, PB4
volatile unsigned char* ddr_b  = (unsigned char*) 0x24;

// Define the registers for pins 53, 51, 49, 47

volatile unsigned char* port_l = (unsigned char*) 0x10B; // PL0, PL1
volatile unsigned char* ddr_l  = (unsigned char*) 0x10A;

// Define the registers for pins 35, 37, 39
volatile unsigned char* port_c = (unsigned char*) 0x28; // PC2, PC4, PC6
volatile unsigned char* ddr_c  = (unsigned char*) 0x27;

// Define the registers for pin 40
volatile unsigned char* port_g = (unsigned char*) 0x34; // PG1
volatile unsigned char* ddr_g  = (unsigned char*) 0x33;


#define RDA 0x80
#define TBE 0x20  



//========================================================================

void setup() {
   U0init(9600);

   adc_init();

  //lil intro screen
  lcd.begin(8, 2); // set up number of columns and rows
  lcd.setCursor(0, 0); // move cursor to (0, 0)
  lcd.print("Hello"); // print message at (0, 0)
  lcd.setCursor(2, 1); // move cursor to (2, 1)
  lcd.print("RIC"); // prpint message at (2, 1)

  //motor
  // Set PB1 (pin 52), PB3 (pin 50), and PB4 (pin 48) to OUTPUT
  *ddr_b |= (1 << 1) | (1 << 3) | (1 << 4);

  //water
  *ddr_g |= (1 << 1);

 // configure D7 pin as an OUTPUT  
  *port_g &= ~(1 << 1);

  //  time
  if (! rtc.begin()) {
   U0putstr("Couldn't find RTC ");
   U0putstr("\n");
    while (1);
  }
  if (! rtc.isrunning()) {
    U0putstr("RTC is NOT running! ");
    U0putstr("\n");
  }
  //LED 
 *ddr_b |= (1 << 0) | (1 << 2);
 *ddr_l |= (1 << 0) | (1 << 1);
  //button
 *ddr_c &= ~((1 << 2) | (1 << 4) | (1 << 6));
  //steppr 
  myStepper.setSpeed(10);
  LCDupdate();
}

void adc_init()
{
  // setup the A register
  *my_ADCSRA |= 0b10000000; // 
  *my_ADCSRA &= 0b11011111; // 
  *my_ADCSRA &= 0b11110111; // 
  *my_ADCSRA &= 0b11111000; // 
}

unsigned int adc_read(unsigned char adc_channel_num)
{

  *my_ADMUX  &= 0b11100000;

  *my_ADCSRB &= 0b11110111;

  if(adc_channel_num > 7)
  {
    adc_channel_num -= 8;
    *my_ADCSRB |= 0b00001000;
  }

  *my_ADMUX  += adc_channel_num;

  *my_ADCSRA |= 0x40;

  while((*my_ADCSRA & 0x40) != 0);

  return *my_ADC_DATA;
}

void U0init(unsigned long U0baud)
{
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}

void U0putstr(char *str) // Modify the function to accept a string
{
    while(*str != '\0') { // Loop until the end of the string
        while(!(*myUCSR0A & (1<<5))); // Wait until the USART is ready
        *myUDR0 = *str; // Send the character
        str++; // Move to the next character in the string
    }
    while (!(*myUCSR0A & (1 << 5))); // Wait until the USART is ready
}

void U0putint(long num) // Function to print a long integer
{
    char buffer[20]; // Buffer to hold the string representation of the number
    ltoa(num, buffer, 10); // Convert the number to a string
    U0putstr(buffer); // Print the string
}

bool emergenC() {
      *port_g |= (1 << 1);

      myDelay(5); // wait 10 milliseconds
      value = adc_read(5);

      if (value <= 0){
      *port_g &= ~(1 << 1);

      return true;
      }else
      {
      return false;
      }
}

void controlVent(Stepper& myStepper) {
  int val = adc_read(4);
  bool check = false;
 

  if(val <= 341 ){
    while (val < 341 )
    {
      myStepper.step(200);
      val = adc_read(4);
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
      val = adc_read(4);
      check = emergenC();
      if(check != false);
      {
        break;
      }
    }
  }
}

void controlVent2(Stepper& myStepper) {
  int val = adc_read(4);
  bool check = false;


  if(val <= 341 ){
    while (val < 341 )
    {
      myStepper.step(10);
      val = adc_read(4);
      
    }
  } else if(val >=682 ){
    while (val > 682)
    {
      myStepper.step(-10);
      val =adc_read(4);
      
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

  U0putstr("Date & Time: ");
  U0putint(now.year());
  U0putstr(" ");
  U0putint(now.month());
    U0putstr(" ");
  U0putint(now.day());
    U0putstr(" ");
  U0putint(now.hour());
    U0putstr(" ");
  U0putint(now.minute());
    U0putstr(" ");
  U0putint(now.second());
  U0putstr("\n");
}
void fan() {
// Set PB1 (pin 52) to high
  *port_b |= (1 << 1);

// Set PB3 (pin 50) to high
  *port_b |= (1 << 3);

// Set PB4 (pin 48) to low
  *port_b &= ~(1 << 4);
	
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
        //U0putstr("Hello World");

    if(millis() >= time_now + period){
        time_now += period;
          LCDupdate();
    }
  
  // printDateTime(); 
   // Call the function to print the date and time
  buttonState1 = digitalRead(35);
  buttonState2 = digitalRead(37);
  buttonState3 = digitalRead(39);
 myDelay(50);
  
  switch (state) {
    case DISABLED:
    {

        lcd.setCursor(0, 0); 
        lcd.print("DISABLED");
      // no checking sensors, ISR start button, Yellow LED
        *port_l |= (1 << 0);

        controlVent2(myStepper);

        

        //=======================================================
        if (buttonState1 == LOW) { //do ISR eventually
        
          *port_l &= ~(1 << 0);
          printDateTime(); 
           
          *port_g |= (1 << 1); // turn the sensor ON
          myDelay(5); // wait 10 milliseconds
          value =  adc_read(5); // read the analog value from sensor
          *port_g &= ~(1 << 1); // turn the sensor OFF
          U0putstr("\n");
          U0putstr("water value: " );
          U0putint(value);
          U0putstr("\n");
          int chk = DHT.read11(DHT11_PIN);
          U0putstr("Temperature = ");
          U0putint(DHT.temperature);
          myDelay(50);
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
      *port_b |= (1 << 0);
      *port_g |= (1 << 1); // turn the sensor ON
      myDelay(5); // wait 10 milliseconds
      value = adc_read(5);
      *port_g &= ~(1 << 1);

       if (buttonState2 == LOW)
            {
                printDateTime();
                state = DISABLED;
                *port_b &= ~(1 << 0);
            }

      if(value <= 1){
            printDateTime(); 
            state = ERROR;
            *port_b &= ~(1 << 0);
          }
          else if(DHT.temperature >= 24){
            printDateTime(); 
            state = RUNNING;
            *port_b &= ~(1 << 0);
          }
    }
      break;
    case ERROR:
    {
      // Motor not on at all, Red LED, LCD display error, RESET BUTTON => IDLE iff water >=100
        *port_b |= (1 << 2);

        lcd.setCursor(0, 0); 
        lcd.print("ERROR   "); 
        if (buttonState2 == LOW)
            {
               printDateTime();
                state = DISABLED;
                *port_b &= ~(1 << 2);
            }
        if (buttonState3 == LOW)
            {
          *port_g |= (1 << 1);// turn the sensor ON
          myDelay(5); // wait 10 milliseconds
          value = adc_read(5); // read the analog value from sensor
          *port_g &= ~(1 << 1); // turn the sensor OFF

            if(value >= 1){
             printDateTime(); 
             state = IDLE;
             *port_b &= ~(1 << 0);
             *port_b &= ~(1 << 2);
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
        *port_l |= (1 << 1);



        if (buttonState2 == LOW)
            {
               printDateTime();
                state = DISABLED;
                // Set PL1 (pin 47) to low
                *port_l &= ~(1 << 1);

                // Set PB3 (pin 50) to low
                *port_b &= ~(1 << 3);
            myDelay(5);
            break;

            }



      
      *port_g |= (1 << 1);
      myDelay(5); // wait 10 milliseconds
      value =  adc_read(5);;
      *port_g &= ~(1 << 1);

        fan();

       if(value <= 1){
            printDateTime(); 
            state = ERROR;
             *port_l &= ~(1 << 1);

             *port_b &= ~(1 << 3);
            myDelay(5);
          }
          else if(DHT.temperature <= 21){
            printDateTime(); 
            state = IDLE;
             *port_l &= ~(1 << 1);

             *port_b &= ~(1 << 3);
            myDelay(5);
          }


    }
      break;
  }
  
  myDelay(500);
}