//modified by Erick Sanchez
//CPE301 3/20

#include <LiquidCrystal.h>
#include <Keypad.h>

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {48, 46, 44, 42}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {40, 38, 36, 34}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// LCD pins <--> Arduino pins
const int RS = 11, EN = 12, D4 = 2, D5 = 3, D6 = 4, D7 = 5;
int right=0,up=0;
int dir1=0,dir2=0;
byte customChar[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

byte customChar2[8] = {
  0b00000,
  0b00000,
  0b01110,
  0b01110,
  0b01110,
  0b00000,
  0b00000,
  0b00000
};
byte clear[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2); // set up number of columns and rows
  lcd.createChar(0, clear);

  lcd.createChar(1, customChar); // create a new custom character
  lcd.setCursor(0, 0); // move cursor to (2, 0)
  lcd.write((byte)1);  // print the custom char at (2, 0)
  
  lcd.createChar(2, customChar2); // create a new custom character
  lcd.setCursor(5, 1); // move cursor to (2, 0)
  lcd.write((byte)2);  // print the custom char at (2, 0)

  lcd.setCursor(9, 1); // move cursor to (2, 0)
  lcd.write((byte)2);  // print the custom char at (2, 0)

  lcd.setCursor(6, 0); // move cursor to (2, 0)
  lcd.write((byte)2);  // print the custom char at (2, 0)
  lcd.setCursor(15, 1); // move cursor to (2, 0)
  lcd.write((byte)2);  // print the custom char at (2, 0)

  lcd.setCursor(0, 0);
}
bool a,b,c,d = false;
int ver = 0, col = 0;

void loop()
{
  char key = keypad.getKey();

if(!a && (col == 5 && ver == 1))
{
  Serial.println("You scored!");
  a = true;
}

if(!b && (col == 9 && ver == 1))
{
  Serial.println("You scored!");
  b = true;
}

if(!c && (col == 6 && ver == 0))
{
  Serial.println("You scored!");
  c = true;
}

if(!d && (col == 15 && ver == 1))
{
  Serial.println("You scored!");
  d = true;
}

  if (key){
    Serial.println(key);

    switch (key) {
      case '2':
        if(ver == 0){
          //ignore
        }else {
          lcd.setCursor(col, ver);
          lcd.write((byte)0);
          ver -= 1;
          lcd.setCursor(col, ver);
          lcd.write((byte)1);
        }
        break;
      case '4':
        if(col == 0){
          lcd.setCursor(col, ver);
          lcd.write((byte)0);
          col = 15;
          lcd.setCursor(col, ver);
          lcd.write((byte)1);
        }else {
          lcd.setCursor(col, ver);
          lcd.write((byte)0);
          col -= 1;
          lcd.setCursor(col, ver);
          lcd.write((byte)1);
        }
        break;
      case '6':
        if(col == 15){
          lcd.setCursor(col, ver);
          lcd.write((byte)0);
          col = 0;
          lcd.setCursor(col, ver);
          lcd.write((byte)1);
        }else {
          lcd.setCursor(col, ver);
          lcd.write((byte)0);
          col += 1;
          lcd.setCursor(col, ver);
          lcd.write((byte)1);
        }
        break;
      case '8':
        if(ver == 1){
          //ignore
        }else {
          lcd.setCursor(col, ver);
          lcd.write((byte)0);
          ver += 1;
          lcd.setCursor(col, ver);
          lcd.write((byte)1);
        }
        break;
      default:
        // statements
        break;
    }
  }
}