#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "RTClib.h"

const int ZONE1_ADC = A11;
const int ZONE2_ADC = A12;
const int NAC_ADC = A13;
const int RAC_ADC = A14;
const int NAC_VCC = 22;
const int RAC_VCC = 24;
const int NAC_PO = 26;
const int RAC_PO = 28;
const int RELAY_PO = 30;
const int INPUT_PI = 32;
int ZONE1_ADC_value = 0;
int ZONE2_ADC_value = 0;
int NAC_ADC_value = 0;
int RAC_ADC_value = 0;
int INPUT_state = 0;
int ZONE_1_SHORT = 23;
int ZONE_1_FIRE = 25;
int ZONE_1_OPEN = 27;
int ZONE_2_SHORT = 29;
int ZONE_2_FIRE = 31;
int ZONE_2_OPEN = 33;
int RAC_FAULT = 35;
int NAC_FAULT = 37;
int BATTERY_FAULT = 39;
int RAC_RELEASE = 41;
int PRE_RELEASE = 43;
int SYSTEM_ON = 45;


bool RAC_FAULT_ALERT = false;
bool NAC_FAULT_ALERT = false;
bool BATTERY_FAULT_ALERT = false;
bool RAC_RELEASE_ALERT = false;
bool PRE_RELEASE_ALERT = false;
bool SYSTEM_ON_ALERT = false;

const int maxAlerts = 10; // Maximum number of alerts that can be queued
char alertQueue[maxAlerts][16]; // Queue to store alerts
int alertQueueStart = 0; // Points to the start of the queue
int alertQueueEnd = 0; // Points to the end of the queue

bool zone1OpenAlert = false; // Global flag for ZONE1 : OPEN alert
bool zone1FireAlert = false;   // Flag for "ZONE1 : FIRE"
bool zone1ShortAlert = false;  // Flag for "ZONE1 : SHORT"

bool zone2OpenAlert = false; // Global flag for ZONE1 : OPEN alert
bool zone2FireAlert = false;   // Flag for "ZONE1 : FIRE"
bool zone2ShortAlert = false;  // Flag for "ZONE1 : SHORT"

bool displayNeedsUpdate = true; // Global variable to track if the display needs updating

bool isAlertActive = false; // Global variable to track the state of the alert
char alertMessage[16] = ""; // Global variable to hold the alert message
unsigned long alertStartTime = 0; // Time when the alert started
const unsigned long alertInterval = 1000; // Interval for alert beep and display swap

RTC_DS3231 rtc;


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

  if (key == '#') {
    dequeueAlert();
    return;
  }

  if (key != NO_KEY) {
        beepBuzzer(); // Beep the buzzer on any keypress
    }
    
  if (currentMenu == HOME_SCREEN && key == 'C') {
        currentMenu = MAIN_MENU;
        updateDisplay();
        return;
    }

    if (key == '1') {
        // Trigger "Alert Message 1"
        isAlertActive = true;
        strncpy(alertMessage, "Alert Message 1", sizeof(alertMessage));
        alertStartTime = millis(); // Reset the timer for the buzzer
    } else if (key == '2') {
        // Trigger "Alert Message 2"
        isAlertActive = true;
        strncpy(alertMessage, "Alert Message 2", sizeof(alertMessage));
        alertStartTime = millis(); // Reset the timer for the buzzer
    } else if (key == '#') {
        isAlertActive = false;
        digitalWrite(buzzerPin, LOW);
        alertMessage[0] = '\0'; 
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
  static unsigned long lastTimeUpdate = 0;
  const unsigned long timeUpdateInterval = 4000; // 4 seconds in milliseconds
  static bool firstUpdateDone = false;
  lcd.clear();

  if (isAlertActive) {
    lcd.setCursor(0, 0); // Set cursor to the beginning of the first line
    lcd.print(alertMessage); // Print the alert message
    return;
  }
  
    if (currentMenu == HOME_SCREEN) {
    unsigned long currentMillis = millis();
    static char dateTime[20] = "Loading..."; // Static variable to retain the value

    if (currentMillis - lastTimeUpdate >= timeUpdateInterval) {
      lastTimeUpdate = currentMillis;
      DateTime now = rtc.now(); // Get current date and time

      sprintf(dateTime, "%02d/%02d/%02d %02d:%02d", now.day(), now.month(), now.year() % 100, now.hour(), now.minute());
    }

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

// Function to add an alert to the queue
void enqueueAlert(const char* alert) {
  if ((alertQueueEnd + 1) % maxAlerts != alertQueueStart) { // Check if queue is not full
    strncpy(alertQueue[alertQueueEnd], alert, 16);
    alertQueueEnd = (alertQueueEnd + 1) % maxAlerts;
    if (!isAlertActive) {
      isAlertActive = true;
      strncpy(alertMessage, alertQueue[alertQueueStart], sizeof(alertMessage));
      displayNeedsUpdate = true;
    }
  }
}

// Function to remove an alert from the queue
void dequeueAlert() {
  if (alertQueueStart != alertQueueEnd) { // Check if queue is not empty
    alertQueueStart = (alertQueueStart + 1) % maxAlerts;
    if (alertQueueStart == alertQueueEnd) {
      isAlertActive = false;
      alertMessage[0] = '\0';
    } else {
      strncpy(alertMessage, alertQueue[alertQueueStart], sizeof(alertMessage));
    }
    displayNeedsUpdate = true;
  }
}

void hwMonitor() {
  ZONE1_ADC_value = analogRead(ZONE1_ADC);
  ZONE2_ADC_value = analogRead(ZONE2_ADC);
  RAC_ADC_value = analogRead(RAC_ADC);
  NAC_ADC_value = analogRead(NAC_ADC);

  digitalWrite(BATTERY_FAULT, HIGH); // Turn ON ZONE1 OPEN LED
  digitalWrite(SYSTEM_ON, HIGH); // Turn ON ZONE1 OPEN LED


  // ZONE1 : OPEN
  if (ZONE1_ADC_value < 20) {
    if (!zone1OpenAlert) {
      zone1OpenAlert = true;
      enqueueAlert("ZONE1 : OPEN");
    }
    digitalWrite(ZONE_1_OPEN, HIGH); // Turn ON ZONE1 OPEN LED
  } else {
    if (zone1OpenAlert) {
      zone1OpenAlert = false;
    }
    digitalWrite(ZONE_1_OPEN, LOW); // Turn OFF ZONE1 OPEN LED
  }

  // ZONE1 : FIRE
  if (ZONE1_ADC_value >= 100 && ZONE1_ADC_value < 800) {
    if (!zone1FireAlert) {
      zone1FireAlert = true;
      enqueueAlert("ZONE1 : FIRE");
    }
    digitalWrite(ZONE_1_FIRE, HIGH); // Turn ON ZONE1 FIRE LED
  } else {
    if (zone1FireAlert) {
      zone1FireAlert = false;
    }
    digitalWrite(ZONE_1_FIRE, LOW); // Turn OFF ZONE1 FIRE LED
  }

  // ZONE1 : SHORT
  if (ZONE1_ADC_value >= 800) {
    if (!zone1ShortAlert) {
      zone1ShortAlert = true;
      enqueueAlert("ZONE1 : SHORT");
    }
    digitalWrite(ZONE_1_SHORT, HIGH); // Turn ON ZONE1 SHORT LED
  } else {
    if (zone1ShortAlert) {
      zone1ShortAlert = false;
    }
    digitalWrite(ZONE_1_SHORT, LOW); // Turn OFF ZONE1 SHORT LED
  }

  // Zone 2 Alert Monitoring
    // ZONE2 : OPEN
    if (ZONE2_ADC_value < 20) {
        if (!zone2OpenAlert) {
            zone2OpenAlert = true;
            enqueueAlert("ZONE2 : OPEN");
        }
        digitalWrite(ZONE_2_OPEN, HIGH); // Turn ON ZONE2 OPEN LED
    } else {
        if (zone2OpenAlert) {
            zone2OpenAlert = false;
        }
        digitalWrite(ZONE_2_OPEN, LOW); // Turn OFF ZONE2 OPEN LED
    }

    // ZONE2 : FIRE
    if (ZONE2_ADC_value >= 100 && ZONE2_ADC_value < 800) {
        if (!zone2FireAlert) {
            zone2FireAlert = true;
            enqueueAlert("ZONE2 : FIRE");
        }
        digitalWrite(ZONE_2_FIRE, HIGH); // Turn ON ZONE2 FIRE LED
    } else {
        if (zone2FireAlert) {
            zone2FireAlert = false;
        }
        digitalWrite(ZONE_2_FIRE, LOW); // Turn OFF ZONE2 FIRE LED
    }

    // ZONE2 : SHORT
    if (ZONE2_ADC_value >= 800) {
        if (!zone2ShortAlert) {
            zone2ShortAlert = true;
            enqueueAlert("ZONE2 : SHORT");
        }
        digitalWrite(ZONE_2_SHORT, HIGH); // Turn ON ZONE2 SHORT LED
    } else {
        if (zone2ShortAlert) {
            zone2ShortAlert = false;
        }
        digitalWrite(ZONE_2_SHORT, LOW); // Turn OFF ZONE2 SHORT LED
    }

        // RAC Monitoring
    if (RAC_ADC_value > 900) {
        if (!RAC_FAULT_ALERT) {
            RAC_FAULT_ALERT = true;
            enqueueAlert("RAC : OPEN");
        }
        digitalWrite(RAC_FAULT, HIGH); // Turn ON RAC FAULT LED
    } else if (RAC_ADC_value < 300) {
        if (!RAC_FAULT_ALERT) {
            RAC_FAULT_ALERT = true;
            enqueueAlert("RAC : SHORT");
        }
        digitalWrite(RAC_FAULT, HIGH); // Turn ON RAC FAULT LED
    } else {
        if (RAC_FAULT_ALERT) {
            RAC_FAULT_ALERT = false;
            digitalWrite(RAC_FAULT, LOW); // Turn OFF RAC FAULT LED
        }
    }

    // NAC Monitoring
    if (NAC_ADC_value > 900) {
        if (!NAC_FAULT_ALERT) {
            NAC_FAULT_ALERT = true;
            enqueueAlert("NAC : OPEN");
        }
        digitalWrite(NAC_FAULT, HIGH); // Turn ON NAC FAULT LED
    } else if (NAC_ADC_value < 300) {
        if (!NAC_FAULT_ALERT) {
            NAC_FAULT_ALERT = true;
            enqueueAlert("NAC : SHORT");
        }
        digitalWrite(NAC_FAULT, HIGH); // Turn ON NAC FAULT LED
    } else {
        if (NAC_FAULT_ALERT) {
            NAC_FAULT_ALERT = false;
            digitalWrite(NAC_FAULT, LOW); // Turn OFF NAC FAULT LED
        }
    }
}

void hwSetup(){
  pinMode(NAC_VCC, OUTPUT);
  digitalWrite(NAC_VCC, HIGH);
  pinMode(RAC_VCC, OUTPUT);
  digitalWrite(RAC_VCC, HIGH);
  pinMode(NAC_PO, OUTPUT);
  pinMode(RAC_PO, OUTPUT);
  pinMode(RELAY_PO, OUTPUT);
  pinMode(INPUT_state, INPUT);
}

void setup() {
  Serial.begin(57600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ZONE_1_SHORT, OUTPUT);
  pinMode(ZONE_1_FIRE, OUTPUT);
  pinMode(ZONE_1_OPEN, OUTPUT);
  pinMode(ZONE_2_SHORT, OUTPUT);
  pinMode(ZONE_2_FIRE, OUTPUT);
  pinMode(ZONE_2_OPEN, OUTPUT);
  pinMode(RAC_FAULT, OUTPUT);
  pinMode(NAC_FAULT, OUTPUT);
  pinMode(BATTERY_FAULT, OUTPUT);
  pinMode(RAC_RELEASE, OUTPUT);
  pinMode(PRE_RELEASE, OUTPUT);
  pinMode(SYSTEM_ON, OUTPUT);
  hwSetup();
  // LCD and keypad initialization (omitted for brevity)
  lcd.init();
  lcd.backlight();
  pushMenu(currentMenu); // Initialize the menu history 
  currentMenu = HOME_SCREEN;
  updateDisplay(); // Initial display update

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    handleKeyPress(key);
//    updateDisplay();
    displayNeedsUpdate = true;
  }

  if (displayNeedsUpdate) {
    updateDisplay();
    displayNeedsUpdate = false; // Reset the flag after updating the display
  }

  if (isAlertActive) {
    unsigned long currentMillis = millis();
    if (currentMillis - alertStartTime >= 1000) { // 1 minute = 60,000 milliseconds
      alertStartTime = currentMillis;
      digitalWrite(buzzerPin, !digitalRead(buzzerPin));
    }
  }
  hwMonitor();
}
