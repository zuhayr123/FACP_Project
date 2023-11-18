#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

char lastSelectedMenu[20]; // Assuming maximum menu name length is 20 characters
const int buzzerPin = 10;
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

char leftSection[] = "Z1:Nrml"; // Placeholder for left section content
char rightSection[] = "Z2:Nrml"; // Placeholder for right section content
char dateTime[] = "07/11/23 12:18"; // Placeholder for date and time


enum MenuState { 
    HOME_SCREEN, MAIN_MENU, INPUT_CONFIG, ZONE_SETTING, ZONE_1, ZONE_2, ZONE_3, ZONE_4, 
    PRESSURE_SWITCH_CONFIG, RAC_1, RAC_2, 
    OUTPUT_CONFIG, RAC_SETTING, RAC_1_SETTING, RAC_2_SETTING, 
    NAC_SETTING, NAC_1_SETTING, NAC_2_SETTING, 
    RELAY_SETTING, RELAY_1_SETTING, RELAY_2_SETTING,
    TIMER_DELAY_SETTING, DATE_TIME_SETTING, AUTO_SILENCE_SETTING, 
    CHIME_SETTING, PASSWORD_SETTING, PANEL_INFO_SETTING, 
    HISTORY, FACTORY_RESET
};




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

// Forward declarations for new action functions
void goToOutputConfig();
void goToRACSetting();
void goToRAC1Setting();
void rac1SettingZ1Z2();
void rac1SettingZ1orZ2();
void rac1SettingZ1();
void rac1SettingZ2();

// Forward declarations for new action functions
void goToRAC2Setting();
void rac2SettingZ3Z4();
void rac2SettingZ3orZ4();
void rac2SettingZ3();
void rac2SettingZ4();

// Forward declarations for new action functions
void goToNACSetting();
void goToNAC1Setting();
void nac1SettingRAC1();
void nac1SettingCommon();

// Forward declarations for new action functions
void goToNAC2Setting();
void nac2SettingRAC2();
void nac2SettingCommonNAC2();

// Forward declarations for new action functions
void goToRelaySetting();
void goToRelay1Setting();
void goToRelay2Setting();
void relay1SettingZ1();
void relay1SettingZ2();
void relay1SettingZ1Z2();
void relay2SettingZ3();
void relay2SettingZ4();
void relay2SettingZ3Z4();

// Forward declarations for new action functions
void goToTimerDelaySetting();
void goToDateTimeSetting();
void goToAutoSilenceSetting();
void goToChimeSetting();
void goToPasswordSetting();
void goToPanelInfoSetting();
void goToHistory();
void goToFactoryReset();

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


MenuItem mainMenuItems[] = {
    {"Input Config", goToInputConfig}, 
    {"Output Config", goToOutputConfig}, 
    {"Timer/Delay Setting", goToTimerDelaySetting}, 
    {"Date/Time Setting", goToDateTimeSetting}, 
    {"Auto Silence Setting", goToAutoSilenceSetting}, 
    {"Chime Setting", goToChimeSetting}, 
    {"Password Setting", goToPasswordSetting}, 
    {"Panel Info Setting", goToPanelInfoSetting}, 
    {"History", goToHistory}, 
    {"Factory Reset", goToFactoryReset}
};
MenuItem zoneSettingItems[] = {{"Zone 1", goToZone1}, {"Zone 2", goToZone2}, {"Zone 3", goToZone3}, {"Zone 4", goToZone4}};
MenuItem zone1Items[] = {{"Location", zone1Location}, {"Enable/Disable", zone1EnableDisable}};
MenuItem zone2Items[] = {{"Location", zone2Location}, {"Enable/Disable", zone2EnableDisable}};
MenuItem zone3Items[] = {{"Location", zone3Location}, {"Enable/Disable", zone3EnableDisable}};
MenuItem zone4Items[] = {{"Location", zone4Location}, {"Enable/Disable", zone4EnableDisable}};

MenuItem outputConfigItems[] = {{"RAC Setting", goToRACSetting}, {"NAC Setting", goToNACSetting}, {"Relay Setting", goToRelaySetting}};
const int outputConfigSize = sizeof(outputConfigItems) / sizeof(MenuItem);

MenuItem racSettingItems[] = {{"RAC 1 Setting", goToRAC1Setting}, {"RAC 2 Setting", goToRAC2Setting}};
const int racSettingSize = sizeof(racSettingItems) / sizeof(MenuItem);

MenuItem rac1SettingItems[] = {{"Z1 & Z2", rac1SettingZ1Z2}, {"Z1 or Z2", rac1SettingZ1orZ2}, {"Z1", rac1SettingZ1}, {"Z2", rac1SettingZ2}};
const int rac1SettingSize = sizeof(rac1SettingItems) / sizeof(MenuItem);

MenuItem rac2SettingItems[] = {{"Z3 & Z4", rac2SettingZ3Z4}, {"Z3 or Z4", rac2SettingZ3orZ4}, {"Z3", rac2SettingZ3}, {"Z4", rac2SettingZ4}};
const int rac2SettingSize = sizeof(rac2SettingItems) / sizeof(MenuItem);

MenuItem nacSettingItems[] = {{"NAC 1 Setting", goToNAC1Setting}, {"NAC 2 Setting", goToNAC2Setting}};
const int nacSettingSize = sizeof(nacSettingItems) / sizeof(MenuItem);

MenuItem nac2SettingItems[] = {{"RAC 2", nac2SettingRAC2}, {"Common", nac2SettingCommonNAC2}};
const int nac2SettingSize = sizeof(nac2SettingItems) / sizeof(MenuItem);

MenuItem nac1SettingItems[] = {{"RAC 1", nac1SettingRAC1}, {"Common", nac1SettingCommon}};
const int nac1SettingSize = sizeof(nac1SettingItems) / sizeof(MenuItem);

MenuItem relaySettingItems[] = {{"Relay 1 Setting", goToRelay1Setting}, {"Relay 2 Setting", goToRelay2Setting}};
const int relaySettingSize = sizeof(relaySettingItems) / sizeof(MenuItem);

MenuItem relay1SettingItems[] = {{"Z1", relay1SettingZ1}, {"Z2", relay1SettingZ2}, {"Z1 & Z2", relay1SettingZ1Z2}};
const int relay1SettingSize = sizeof(relay1SettingItems) / sizeof(MenuItem);

MenuItem relay2SettingItems[] = {{"Z3", relay2SettingZ3}, {"Z4", relay2SettingZ4}, {"Z3 & Z4", relay2SettingZ3Z4}};
const int relay2SettingSize = sizeof(relay2SettingItems) / sizeof(MenuItem);

const int mainMenuSize = sizeof(mainMenuItems) / sizeof(MenuItem);
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
  strcpy(lastSelectedMenu, "Input Config");
  pushMenu(currentMenu);
  currentMenu = INPUT_CONFIG;
  currentIndex = 0;
}

void goToZoneSetting() {
  strcpy(lastSelectedMenu, "Zone Setting");
  pushMenu(currentMenu);
  currentMenu = ZONE_SETTING;
  currentIndex = 0;
}

void goToOutputConfig() {
  strcpy(lastSelectedMenu, "Output Config");
  pushMenu(currentMenu);
  currentMenu = OUTPUT_CONFIG;
  currentIndex = 0;
}

void goToRACSetting() {
  strcpy(lastSelectedMenu, "RAAC Config");
  pushMenu(currentMenu);
  currentMenu = RAC_SETTING;
  currentIndex = 0;
}

void goToRAC1Setting() {
  strcpy(lastSelectedMenu, "RAC1 Config");
  pushMenu(currentMenu);
  currentMenu = RAC_1_SETTING;
  currentIndex = 0;
}

void goToRAC2Setting() {
  strcpy(lastSelectedMenu, "RAC2 Config");
  pushMenu(currentMenu);
  currentMenu = RAC_2_SETTING;
  currentIndex = 0;
}

void goToNACSetting() {
  strcpy(lastSelectedMenu, "NAC Config");
  pushMenu(currentMenu);
  currentMenu = NAC_SETTING;
  currentIndex = 0;
}

void goToNAC1Setting() {
  strcpy(lastSelectedMenu, "NAC1 Config");
  pushMenu(currentMenu);
  currentMenu = NAC_1_SETTING;
  currentIndex = 0;
}

void goToNAC2Setting() {
  strcpy(lastSelectedMenu, "NAC2 Config");
  pushMenu(currentMenu);
  currentMenu = NAC_2_SETTING;
  currentIndex = 0;
}

void goToRelaySetting() {
  strcpy(lastSelectedMenu, "Relay Config");
  pushMenu(currentMenu);
  currentMenu = RELAY_SETTING;
  currentIndex = 0;
}

// Implement the action functions for new menu items
void goToTimerDelaySetting() { /* Implement Action */ }
void goToDateTimeSetting() { /* Implement Action */ }
void goToAutoSilenceSetting() { /* Implement Action */ }
void goToChimeSetting() { /* Implement Action */ }
void goToPasswordSetting() { /* Implement Action */ }
void goToPanelInfoSetting() { /* Implement Action */ }
void goToHistory() { /* Implement Action */ }
void goToFactoryReset() { /* Implement Action */ }

void goToRelay1Setting() { pushMenu(currentMenu); currentMenu = RELAY_1_SETTING; currentIndex = 0; }
void goToRelay2Setting() { pushMenu(currentMenu); currentMenu = RELAY_2_SETTING; currentIndex = 0; }

// Implement the action functions for Relay 1 and Relay 2 Settings
void relay1SettingZ1() { /* Implement Action */ }
void relay1SettingZ2() { /* Implement Action */ }
void relay1SettingZ1Z2() { /* Implement Action */ }
void relay2SettingZ3() { /* Implement Action */ }
void relay2SettingZ4() { /* Implement Action */ }
void relay2SettingZ3Z4() { /* Implement Action */ }

// Implement the action functions for NAC 2 Settings
void nac2SettingRAC2() { /* Implement Action for NAC 2 - RAC 2 */ }
void nac2SettingCommonNAC2() { /* Implement Action for NAC 2 - Common */ }

// Implement the action functions for NAC 1 Settings
void nac1SettingRAC1() { /* Implement Action */ }
void nac1SettingCommon() { /* Implement Action */ }

// Implement the action functions for RAC 2 Settings
void rac2SettingZ3Z4() { /* Implement Action */ }
void rac2SettingZ3orZ4() { /* Implement Action */ }
void rac2SettingZ3() { /* Implement Action */ }
void rac2SettingZ4() { /* Implement Action */ }

// Implement the action functions for RAC 1 Settings
void rac1SettingZ1Z2() { /* Implement Action */ }
void rac1SettingZ1orZ2() { /* Implement Action */ }
void rac1SettingZ1() { /* Implement Action */ }
void rac1SettingZ2() { /* Implement Action */ }

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
  if (key == 'B' && currentMenu == MAIN_MENU) {
        currentMenu = HOME_SCREEN;
        updateDisplay();
        return;
    }
    
  switch (key) {
    case '4': // Scroll Left
      if (currentIndex > 0) {
        currentIndex--;
      }
      break;
    case '6': // Scroll Right
      if (currentIndex < menuSize - 1) {
        currentIndex++;
      }
      break;
    case 'A': // Select
      menuItems[currentIndex].action();
      break;
    case 'B': // Go Back
      if (menuHistoryPointer >= 0) {
        currentMenu = menuHistory[menuHistoryPointer--];
        currentIndex = 0;
      }
      break;
  }
}


void displayMenu(MenuItem* menuItems, int menuSize) {
    lcd.clear();

    // Dynamically display the name of the current menu on the first line
    switch (currentMenu) {
        case MAIN_MENU:
            lcd.print("Main Menu");
            break;
        case INPUT_CONFIG:
            lcd.print("Input Config");
            break;
        case OUTPUT_CONFIG:
            lcd.print("Output Config");
            break;
        // Add cases for other menus as needed
        // ...
        default:
            lcd.print("Menu"); // Fallback or generic title
    }

    // Second line: Display the current submenu item with conditional arrows
    String secondLine = " ";  // Start with a space for alignment
    int availableChars = 14; // 2 chars are reserved for '<' and '>'

    // Add left arrow if not at the first item
    if (currentIndex > 0) {
        secondLine[0] = '<';
    }

    String menuItem = menuItems[currentIndex].name;
    // Truncate the menu item if it's too long
    if (menuItem.length() > availableChars) {
        menuItem = menuItem.substring(0, availableChars - 2) + "..";
    }
    secondLine += menuItem;

    // Pad with spaces to ensure right alignment
    while (secondLine.length() < 15) {
        secondLine += " ";
    }

    // Add right arrow if not at the last item
    if (currentIndex < menuSize - 1) {
        secondLine += ">";
    } else {
        secondLine += " "; // Add space if no right arrow
    }

    // Display the scrollable item on the second line
    lcd.setCursor(0, 1);
    lcd.print(secondLine);
}


void handleKeyPress(char key) {

  if (key != NO_KEY) {
        beepBuzzer(); // Beep the buzzer on any keypress
    }
    
  if (currentMenu == HOME_SCREEN && key == 'C') {
        currentMenu = MAIN_MENU;
        updateDisplay();
        return;
    }
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
    case OUTPUT_CONFIG:
      navigateMenu(outputConfigItems, outputConfigSize, key);
      break;
    case RAC_SETTING:
      navigateMenu(racSettingItems, racSettingSize, key);
      break;
    case RAC_1_SETTING:
      navigateMenu(rac1SettingItems, rac1SettingSize, key);
      break;
    case RAC_2_SETTING:
      navigateMenu(rac2SettingItems, rac2SettingSize, key);
      break;
    case NAC_SETTING:
      navigateMenu(nacSettingItems, nacSettingSize, key);
      break;
    case NAC_1_SETTING:
      navigateMenu(nac1SettingItems, nac1SettingSize, key);
      break;
    case NAC_2_SETTING:
      navigateMenu(nac2SettingItems, nac2SettingSize, key);
      break;
    case RELAY_SETTING:
      navigateMenu(relaySettingItems, relaySettingSize, key);
      break;
    case RELAY_1_SETTING:
      navigateMenu(relay1SettingItems, relay1SettingSize, key);
      break;
    case RELAY_2_SETTING:
      navigateMenu(relay2SettingItems, relay2SettingSize, key);
      break;
    // ... other cases
  }
}

void updateDisplay() {
  lcd.clear();
    if (currentMenu == HOME_SCREEN) {
        lcd.setCursor(0, 0); // Set cursor to the first line
        lcd.print(leftSection); // Print the left section content
        lcd.setCursor(9, 0); // Set cursor position for right section
        lcd.print(rightSection); // Print the right section content
        lcd.setCursor(0, 1); // Set cursor to the second line
        lcd.print(dateTime); // Print the date and time
        return;
    }
    
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
    case OUTPUT_CONFIG:
      displayMenu(outputConfigItems, outputConfigSize);
      break;
    case RAC_SETTING:
      displayMenu(racSettingItems, racSettingSize);
      break;
    case RAC_1_SETTING:
      displayMenu(rac1SettingItems, rac1SettingSize);
      break;
    case RAC_2_SETTING:
      displayMenu(rac2SettingItems, rac2SettingSize);
      break;
    case NAC_SETTING:
      displayMenu(nacSettingItems, nacSettingSize);
      break;
    case NAC_1_SETTING:
      displayMenu(nac1SettingItems, nac1SettingSize);
      break;
    case NAC_2_SETTING:
      displayMenu(nac2SettingItems, nac2SettingSize);
      break;
    case RELAY_SETTING:
      displayMenu(relaySettingItems, relaySettingSize);
      break;
    case RELAY_1_SETTING:
      displayMenu(relay1SettingItems, relay1SettingSize);
      break;
    case RELAY_2_SETTING:
      displayMenu(relay2SettingItems, relay2SettingSize);
      break;
    // ... other cases
  }
}

void beepBuzzer() {
    digitalWrite(buzzerPin, HIGH); // Turn buzzer on
    delay(50); // Wait for 50 milliseconds
    digitalWrite(buzzerPin, LOW); // Turn buzzer off
}

void setup() {
  pinMode(buzzerPin, OUTPUT);
  // LCD and keypad initialization (omitted for brevity)
  lcd.init();
  lcd.backlight();
  pushMenu(currentMenu); // Initialize the menu history 
  currentMenu = HOME_SCREEN;
  updateDisplay(); // Initial display update
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    handleKeyPress(key);
    updateDisplay();
  }
}
