#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3}; //connect to the column pinouts of the keypad

//Create an object of keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int wait = 1;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

//LiquidCrystal_I2C lcd(0x38, BACKLIGHT_PIN, POSITIVE);  // Set the LCD I2C address


void setup()
{
  //Serial.begin(9600);
  lcd.begin(16,2);               // initialize the lcd 
  lcd.clear();
  lcd.print("hello, world!");
  delay(1000);
}

void loop()
{
  char key = keypad.getKey();// Read the key
  // Print if key pressed
  if (key){
    lcd.clear();
    lcd.print("Key Pressed : ");
    lcd.print(key);
    delay(1500);
    wait = 1;
  } else {
    if (wait){
      lcd.clear();
      lcd.print("Press a key");
      wait = 0;
    }
  }
}