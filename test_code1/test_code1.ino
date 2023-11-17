#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Keypad setup for 4x4 keypad
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD I2C address

// Menu state machine
enum MenuState { MAIN_MENU, LEVEL_1, LEVEL_2, LEVEL_3 };
MenuState currentMenu = MAIN_MENU;

int selectedItemLevel1 = 0;
int selectedItemLevel2 = 0;
int selectedItemLevel3 = 0;

// Menu items
const char* mainMenuItems[] = {"Input Menu Setting", "Output Menu Setting", "Timer/Delay Setting", "Date/Time Setting", "Auto Silence Setting", "Chime Setting", "Password Setting", "Panel Info Setting", "History View Setting", "Factory Reset"};
const char* level1MenuItems[] = {"SubItem1", "SubItem2", "SubItem3"};
const char* level2MenuItems[] = {"Option1", "Option2"};

void setup() {
  lcd.init();
  lcd.backlight();
  updateDisplay();  // Initial display update
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    handleKeyPress(key);
    updateDisplay();
  }
}

void handleKeyPress(char key) {
  switch (currentMenu) {
    case MAIN_MENU:
      handleMain(key);
      break;
    case LEVEL_1:
      handleLevel1(key);
      break;
    case LEVEL_2:
      handleLevel2(key);
      break;
    case LEVEL_3:
      handleLevel3(key);
      break;
  }
}

void handleMain(char key) {
  if (key == 'A') {
    currentMenu = LEVEL_1;
  }
}

void handleLevel1(char key) {
  switch (key) {
    case '1': selectedItemLevel1 = 0; break;
    case '2': selectedItemLevel1 = 1; break;
    case '3': selectedItemLevel1 = 2; break;
    case 'A': currentMenu = LEVEL_2; break;
    case 'B': currentMenu = MAIN_MENU; break;
  }
}

void handleLevel2(char key) {
  switch (key) {
    case '1': selectedItemLevel2 = 0; break;
    case '2': selectedItemLevel2 = 1; break;
    case '3': selectedItemLevel2 = 2; break;
    case 'A': currentMenu = LEVEL_3; break;
    case 'B': currentMenu = LEVEL_1; break;
  }
}

void handleLevel3(char key) {
  switch (key) {
    case '1': selectedItemLevel3 = 0; break;
    case '2': selectedItemLevel3 = 1; break;
    case 'A': 
      // Perform action for selected item
      break;
    case 'B': currentMenu = LEVEL_2; break;
  }
}

void updateDisplay() {
  lcd.clear();
  switch (currentMenu) {
    case MAIN_MENU:
      lcd.print("Main Menu");
      break;
    case LEVEL_1:
      lcd.print("L1: ");
      lcd.print(mainMenuItems[selectedItemLevel1]);
      break;
    case LEVEL_2:
      lcd.print("L2: ");
      lcd.print(level1MenuItems[selectedItemLevel2]);
      break;
    case LEVEL_3:
      lcd.print("L3: ");
      lcd.print(level2MenuItems[selectedItemLevel3]);
      break;
  }
}
