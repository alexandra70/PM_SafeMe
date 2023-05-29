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

char menu_items[3][16] = {"Log out", "Encrypt", "Decrypt"};
char users[2][16] = {"Oana", "Admin"};
char pins[2][4] = {"1234", "0000"};
char sym_keys[2][8] = {"oanakey1", "adminkey"};

int active_user = -1;
char buffer_read[34];

void setup()
{
  Serial.begin(9600);
  lcd.begin(16,2);               // initialize the lcd 
  lcd.backlight(); // Turn on the LCD backlight
}

void readfromserial()
{
  String msg;

  Serial.println("Please provide input of up to 32 bytes.");
  while (Serial.available() == 0){}

  msg = Serial.readString(); // Read the message from the serial port
  msg.toCharArray(buffer_read, 34);
}


void myprint(char* s1, char* s2)
{
  lcd.clear();
  lcd.print(s1);
  lcd.setCursor(0, 1);
  lcd.print(s2);
}

void login_menu(int user)
{
  myprint(users[user], "#: next *:login");
  char key = keypad.getKey();
  while (!key)
    key = keypad.getKey();
  if (key == '#'){
    user = (user + 1) % 2;
    login_menu(user);
  } else if (key == '*'){
    ask_pin(user);
  } else {
    myprint("Invalid key", "");
    delay(500);
    login_menu(user);
  }
}

void ask_pin(int user)
{
  myprint("Insert PIN", "");
  char inserted_pin[4];
  for (int i = 0; i < 4; i++){
    char key = keypad.getKey();
    while (!key)
      key = keypad.getKey();
    inserted_pin[i] = key;
    lcd.print("*");
  }
  for (int i = 0; i < 4; i++){
    if(pins[user][i] != inserted_pin[i]){
      myprint("Wrong PIN", "");
      delay(500);
      login_menu(user);
    }
  }
  myprint("Welcome,", users[user]);
  active_user = user;
  delay(1000);
  menu(0);
}

void encrypt_message()
{
  myprint("Write plaintext", "to serial");
  readfromserial();
  Serial.println("Received:");
  Serial.println(buffer_read);
  myprint("Thank you", "Encrypting...");

  char cypher[34];
  int i;
  cypher[0] = active_user + 67;
  for(i = 0; buffer_read[i] != 0; i++){
    cypher[i+1] = buffer_read[i] ^ sym_keys[active_user][i % 8];
  }
  cypher[i+1] = 0;
  Serial.println("Encryption:");
  Serial.println(cypher);
  delay(1000);
  myprint("Message","encrypted");
}

void decrypt_message()
{
  myprint("Write cypher", "to serial");
  readfromserial();
  Serial.println("Received:");
  Serial.println(buffer_read);
  myprint("Thank you", "Decrypting...");

  char plain[34];
  int i;
  int key_used = buffer_read[0] - 67;
  if(key_used != active_user && active_user != 1){
    myprint("Unauthorized", "to decrypt");
    delay(1000);
    return;
  }

  for(i = 1; buffer_read[i] != 0; i++){
    plain[i-1] = buffer_read[i] ^ sym_keys[key_used][(i-1) % 8];
  }
  plain[i-1] = 0;
  Serial.println("Decryption:");
  Serial.println(plain);
  delay(1000);
  myprint("Message","decrypted");
}

void menu(int screen)
{
  myprint(menu_items[screen], "#: next *:select");
  char key = keypad.getKey();
  while (!key)
    key = keypad.getKey();
  if (key == '#'){
    screen = (screen + 1) % 3;
    menu(screen);
  } else if (key == '*'){
    if (screen == 0){
      myprint("Goodbye,", users[active_user]);
      active_user = -1;
      delay(1000);
      login_menu(0);
    } else if (screen == 1) {
      encrypt_message();
    } else if (screen == 2) {
      decrypt_message();
    }
    delay(1000);
    menu(screen);
  } else {
    myprint("Invalid key", "");
    delay(1000);
    menu(screen);
  }
}

void loop()
{
  login_menu(0);
}