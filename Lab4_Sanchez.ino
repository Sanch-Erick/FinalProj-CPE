
volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned int  *myTCNT1  = (unsigned  int *) 0x84;
volatile unsigned char *myTIFR1 =  (unsigned char *) 0x36;

volatile unsigned char* port_b = (unsigned char*) 0x25; // Port B address
volatile unsigned char* ddr_b  = (unsigned char*) 0x24; // DDR B address


void setPB6High() {
    *ddr_b |= 0x40; // output
    *port_b |= 0x40; //  high
}

void setPB6Low() {
    *port_b &= 0xBF;// low
}

void my_delay(unsigned int freq)
{
  Serial.print("Frequency: ");
  Serial.println(freq); // Print the received character

  // calc period
  double period = 1.0/double(freq);
  // 50% duty cycle
  double half_period = period/ 2.0f;
  // clock period def
  double clk_period = 0.0000000625;
  // calc ticks
  unsigned int ticks = half_period / clk_period;
  // stop the timer
  *myTCCR1B &= 0xF8;
  // set the counts
  *myTCNT1 = (unsigned int) (65536 - ticks);
  // start the timer
  * myTCCR1A = 0x0;
  * myTCCR1B |= 0b00000101;
  // wait for overflow
  while((*myTIFR1 & 0x01)==0); // 0b 0000 0000
  // stop the timer
  *myTCCR1B &= 0xF8;   // 0b 0000 0000
  // reset TOV           
  *myTIFR1 |= 0x01;
}

void setup() {
    Serial.begin(9600); // Initialize serial communication
}

void loop() {
    if (Serial.available()) {
        char inputChar = Serial.read();
        Serial.print("Received character: ");
        Serial.println(inputChar); 

        int freq = 0.0;
        switch (inputChar) {
            case 'A': freq = 440;setPB6High(); break;
            case 'B': freq = 493;setPB6High();  break;
            case 'C': freq = 523;setPB6High();  break;
            case 'D': freq = 587;setPB6High();  break;
            case 'E': freq = 659;setPB6High();  break;
            case 'F': freq = 698;setPB6High();  break;
            case 'G': freq = 783;setPB6High();  break;
            case 'q': setPB6Low(); return;
        }
        
        my_delay(static_cast<unsigned int>(freq));
    }
}