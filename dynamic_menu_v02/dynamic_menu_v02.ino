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

enum MenuState { MAIN_MENU, INPUT_CONFIG, ZONE_SETTING, ZONE_1, ZONE_2, ZONE_3, ZONE_4, PRESSURE_SWITCH_CONFIG, RAC_1, RAC_2 };

MenuState currentMenu = MAIN_MENU;

int currentIndex = 0; // Current index in the menu

const int maxMenuDepth = 10; // Maximum depth of menu navigation
MenuState menuHistory[maxMenuDepth];
int menuHistoryPointer = -1; // Start with -1 indicating an empty stack

struct MenuItem {
  const char* name;
  void (*action)();
};

// Forward declarations of action functions
void goToInputConfig();
void goToZoneSetting();
void goToZone1();
void goToZone2();
void goToZone3();
void goToZone4();
void zone1Location();
void zone1EnableDisable();
void zone2Location();
void zone2EnableDisable();
void zone3Location();
void zone3EnableDisable();
void zone4Location();
void zone4EnableDisable();

// Forward declarations for new action functions
void goToPressureSwitchConfig();
void goToRAC1();
void goToRAC2();
void rac1Switch();
void rac2Switch();

// Extend the inputConfigItems array
MenuItem inputConfigItems[] = {{"Zone Setting", goToZoneSetting}, {"Pressure Switch Config", goToPressureSwitchConfig}};
const int inputConfigSize = sizeof(inputConfigItems) / sizeof(MenuItem);

MenuItem pressureSwitchItems[] = {{"RAC 1", goToRAC1}, {"RAC 2", goToRAC2}};
const int pressureSwitchSize = sizeof(pressureSwitchItems) / sizeof(MenuItem);

MenuItem rac1Items[] = {{"With/Without SW", rac1Switch}};
MenuItem rac2Items[] = {{"With/Without SW", rac2Switch}};
const int rac1Size = sizeof(rac1Items) / sizeof(MenuItem);
const int rac2Size = sizeof(rac2Items) / sizeof(MenuItem);

void goToPressureSwitchConfig() {
  pushMenu(currentMenu);
  currentMenu = PRESSURE_SWITCH_CONFIG;
  currentIndex = 0;
}

void goToRAC1() { pushMenu(currentMenu); currentMenu = RAC_1; currentIndex = 0; }
void goToRAC2() { pushMenu(currentMenu); currentMenu = RAC_2; currentIndex = 0; }
void rac1Switch() { /* Implement RAC 1 Switch Action */ }
void rac2Switch() { /* Implement RAC 2 Switch Action */ }


MenuItem mainMenuItems[] = {{"Input Config", goToInputConfig}};
//MenuItem inputConfigItems[] = {{"Zone Setting", goToZoneSetting}};
MenuItem zoneSettingItems[] = {{"Zone 1", goToZone1}, {"Zone 2", goToZone2}, {"Zone 3", goToZone3}, {"Zone 4", goToZone4}};
MenuItem zone1Items[] = {{"Location", zone1Location}, {"Enable/Disable", zone1EnableDisable}};
MenuItem zone2Items[] = {{"Location", zone2Location}, {"Enable/Disable", zone2EnableDisable}};
MenuItem zone3Items[] = {{"Location", zone3Location}, {"Enable/Disable", zone3EnableDisable}};
MenuItem zone4Items[] = {{"Location", zone4Location}, {"Enable/Disable", zone4EnableDisable}};

const int mainMenuSize = sizeof(mainMenuItems) / sizeof(MenuItem);
//const int inputConfigSize = sizeof(inputConfigItems) / sizeof(MenuItem);
const int zoneSettingSize = sizeof(zoneSettingItems) / sizeof(MenuItem);
const int zone1Size = sizeof(zone1Items) / sizeof(MenuItem);
const int zone2Size = sizeof(zone2Items) / sizeof(MenuItem);
const int zone3Size = sizeof(zone3Items) / sizeof(MenuItem);
const int zone4Size = sizeof(zone4Items) / sizeof(MenuItem);

void pushMenu(MenuState menu) {
  if (menuHistoryPointer < maxMenuDepth - 1) {
    menuHistory[++menuHistoryPointer] = menu;
  }
}

void goToInputConfig() {
  pushMenu(currentMenu);
  currentMenu = INPUT_CONFIG;
  currentIndex = 0;
}

void goToZoneSetting() {
  pushMenu(currentMenu);
  currentMenu = ZONE_SETTING;
  currentIndex = 0;
}

void goToZone1() { pushMenu(currentMenu); currentMenu = ZONE_1; currentIndex = 0; }
void goToZone2() { pushMenu(currentMenu); currentMenu = ZONE_2; currentIndex = 0; }
void goToZone3() { pushMenu(currentMenu); currentMenu = ZONE_3; currentIndex = 0; }
void goToZone4() { pushMenu(currentMenu); currentMenu = ZONE_4; currentIndex = 0; }

void zone1Location() { /* Implement Zone 1 Location Action */ }
void zone1EnableDisable() { /* Implement Zone 1 Enable/Disable Action */ }
void zone2Location() { /* Implement Zone 2 Location Action */ }
void zone2EnableDisable() { /* Implement Zone 2 Enable/Disable Action */ }
void zone3Location() { /* Implement Zone 3 Location Action */ }
void zone3EnableDisable() { /* Implement Zone 3 Enable/Disable Action */ }
void zone4Location() { /* Implement Zone 4 Location Action */ }
void zone4EnableDisable() { /* Implement Zone 4 Enable/Disable Action */ }

void navigateMenu(MenuItem* menuItems, int menuSize, char key) {
  switch (key) {
    case '4': currentIndex = max(0, currentIndex - 1); break;
    case '6': currentIndex = min(menuSize - 1, currentIndex + 1); break;
    case 'A': menuItems[currentIndex].action(); break;
    case 'B': 
      if (menuHistoryPointer >= 0) {
        currentMenu = menuHistory[menuHistoryPointer--];
        currentIndex = 0;
      }
      break;
  }
}

void displayMenu(MenuItem* menuItems, int menuSize) {
  lcd.clear();

  // Determine the start index for displaying menu items based on the current index and LCD size
  int startIndex = max(0, min(currentIndex, menuSize - 2));  // Adjust -2 based on your LCD size

  for (int i = 0; i < min(2, menuSize - startIndex); i++) {  // Adjust 2 based on your LCD size
    if (startIndex + i == currentIndex) {
      lcd.print("> ");  // Indicate current selection
    } else {
      lcd.print("  ");
    }
    lcd.print(menuItems[startIndex + i].name);
    if (i < min(2, menuSize - startIndex) - 1) {
      lcd.setCursor(0, i + 1);  // Move to the next line
    }
  }
}


void handleKeyPress(char key) {
  switch (currentMenu) {
    case MAIN_MENU:
      navigateMenu(mainMenuItems, mainMenuSize, key);
      break;
    case INPUT_CONFIG:
      navigateMenu(inputConfigItems, inputConfigSize, key);
      break;
    case ZONE_SETTING:
      navigateMenu(zoneSettingItems, zoneSettingSize, key);
      break;
    case ZONE_1:
      navigateMenu(zone1Items, zone1Size, key);
      break;
    case ZONE_2:
      navigateMenu(zone2Items, zone2Size, key);
      break;
    case ZONE_3:
      navigateMenu(zone3Items, zone3Size, key);
      break;
    case ZONE_4:
      navigateMenu(zone4Items, zone4Size, key);
      break;
    case PRESSURE_SWITCH_CONFIG:
      navigateMenu(pressureSwitchItems, pressureSwitchSize, key);
      break;
    case RAC_1:
      navigateMenu(rac1Items, rac1Size, key);
      break;
    case RAC_2:
      navigateMenu(rac2Items, rac2Size, key);
      break;
    // ... other cases
  }
}

void updateDisplay() {
  switch (currentMenu) {
    case MAIN_MENU:
      displayMenu(mainMenuItems, mainMenuSize);
      break;
    case INPUT_CONFIG:
      displayMenu(inputConfigItems, inputConfigSize);
      break;
    case ZONE_SETTING:
      displayMenu(zoneSettingItems, zoneSettingSize);
      break;
    case ZONE_1:
      displayMenu(zone1Items, zone1Size);
      break;
    case ZONE_2:
      displayMenu(zone2Items, zone2Size);
      break;
    case ZONE_3:
      displayMenu(zone3Items, zone3Size);
      break;
    case ZONE_4:
      displayMenu(zone4Items, zone4Size);
      break;
    case PRESSURE_SWITCH_CONFIG:
      displayMenu(pressureSwitchItems, pressureSwitchSize);
      break;
    case RAC_1:
      displayMenu(rac1Items, rac1Size);
      break;
    case RAC_2:
      displayMenu(rac2Items, rac2Size);
      break;
    // ... other cases
  }
}

void setup() {
  // LCD and keypad initialization (omitted for brevity)
  lcd.init();
  lcd.backlight();
  pushMenu(currentMenu); // Initialize the menu history
  updateDisplay(); // Initial display update
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    handleKeyPress(key);
    updateDisplay();
  }
}
