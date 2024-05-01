// CPE 301 - LAB 3
// Written By Erick Sanchez, Spring 2024


volatile unsigned char* port_k = (unsigned char*) 0x108; //PK2
volatile unsigned char* ddr_k  = (unsigned char*) 0x107;
volatile unsigned char* pin_k  = (unsigned char*) 0x106;

volatile unsigned char* port_d = (unsigned char*) 0x2B; // PD0
volatile unsigned char* ddr_d  = (unsigned char*) 0x2A;

volatile unsigned char* port_e = (unsigned char*) 0x2E; // PE3
volatile unsigned char* ddr_e  = (unsigned char*) 0x2D;

void setup() 
{
  Serial.begin(9600);
  
  *port_k |= 0x04; 

  // Set PD0 and PE3 to OUTPUT
  *ddr_d |= 0x01; 
  *ddr_e |= 0x08; 

}

void loop() 
{
  // If the pin is high
  if(*pin_k & 0x04) // Check bit 2 (PK2)
  {
    Serial.println("PIN IS HIGH");
    // Set PD0 to HIGH
    *port_d |= 0x01; 
    // Set PE3 to LOW
    *port_e &= 0xF7; 
  }
  
  // If the pin is low
  else
  {
    Serial.println("PIN IS LOW");
    // Set PD0 to LOW
    *port_d &= 0xFE; 
    // Set PE3 to HIGH
    *port_e |= 0x08;
  }
  delay(25);
}