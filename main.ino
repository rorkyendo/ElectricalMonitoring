#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PZEM004Tv30.h>

#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_ROWS 4

#define NXT_BUTTON 3
#define PRV_BUTTON 4
#define START_BUTTON 5
#define PAUSE_BUTTON 6
#define STOP_BUTTON 7

PZEM004Tv30 pzem(9, 10);

// Inisialisasi objek LCD
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

unsigned long startTime = 0;
unsigned long elapsedTime = 0;
bool isRunning = true;

// Enumeration for different measurement modes
enum MeasurementMode {
  VOLTAGE,
  CURRENT,
  POWER,
  ENERGY,
  FREQUENCY,
  POWER_FACTOR
};

MeasurementMode currentMode = VOLTAGE; // Initial mode

// Pause flags for each measurement
bool isVoltagePaused = false;
bool isCurrentPaused = false;
bool isPowerPaused = false;
bool isEnergyPaused = false;
bool isFrequencyPaused = false;
bool isPowerFactorPaused = false;

// Last measured values
float lastVoltage = 0.0;
float lastCurrent = 0.0;
float lastPower = 0.0;
float lastEnergy = 0.0;
float lastFrequency = 0.0;
float lastPowerFactor = 0.0;

void setup() {
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(PAUSE_BUTTON, INPUT_PULLUP);
  pinMode(STOP_BUTTON, INPUT_PULLUP);
  pinMode(NXT_BUTTON, INPUT_PULLUP);
  pinMode(PRV_BUTTON, INPUT_PULLUP);

  // Inisialisasi LCD
  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  PT BERKAH SUGITRA ");
  lcd.setCursor(0, 1);
  lcd.print("       TEKNIK     ");
  lcd.setCursor(0, 3);
  lcd.print("Memulai Perangkat..");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     -=PT BST=- ");
}

void loop() {
  byte btnStart = digitalRead(START_BUTTON);
  byte btnPause = digitalRead(PAUSE_BUTTON);
  byte btnStop = digitalRead(STOP_BUTTON);
  byte btnNxt = digitalRead(NXT_BUTTON);
  byte btnPrv = digitalRead(PRV_BUTTON);

  if (isRunning && !isVoltagePaused && !isCurrentPaused && !isPowerPaused &&
      !isEnergyPaused && !isFrequencyPaused && !isPowerFactorPaused) {
    unsigned long currentTime = millis();
    elapsedTime += currentTime - startTime;
    startTime = currentTime;
  }

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  displayTime(elapsedTime / 1000);  // Display elapsed time in HH:mm:ss format

  if (btnStart == LOW && !isRunning) {
    isRunning = true;
    startTime = millis();
    lcd.print("     ");
    // Unpause all measurements when starting
    isVoltagePaused = false;
    isCurrentPaused = false;
    isPowerPaused = false;
    isEnergyPaused = false;
    isFrequencyPaused = false;
    isPowerFactorPaused = false;
  } else if (btnPause == LOW && isRunning) {
    elapsedTime += millis() - startTime;
    isRunning = false;
    lcd.print("  PS ");
    // Pause all measurements when pausing
    isVoltagePaused = true;
    isCurrentPaused = true;
    isPowerPaused = true;
    isEnergyPaused = true;
    isFrequencyPaused = true;
    isPowerFactorPaused = true;
  } else if (btnStop == LOW) {
    elapsedTime = 0;
    isRunning = false;
    lcd.print("  STP");
    // Stop and clear all measurements when stopping
    isVoltagePaused = false;
    isCurrentPaused = false;
    isPowerPaused = false;
    isEnergyPaused = false;
    isFrequencyPaused = false;
    isPowerFactorPaused = false;
  }

  if (btnNxt == LOW) {
    switch (currentMode) {
      case VOLTAGE:
        currentMode = CURRENT;
        break;
      case CURRENT:
        currentMode = POWER;
        break;
      case POWER:
        currentMode = ENERGY;
        break;
      case ENERGY:
        currentMode = FREQUENCY;
        break;
      case FREQUENCY:
        currentMode = POWER_FACTOR;
        break;
      case POWER_FACTOR:
        currentMode = VOLTAGE; // Wrap around to the first mode
        break;
    }
  } else if (btnPrv == LOW) {
    switch (currentMode) {
      case VOLTAGE:
        currentMode = POWER_FACTOR; // Wrap around to the last mode
        break;
      case CURRENT:
        currentMode = VOLTAGE;
        break;
      case POWER:
        currentMode = CURRENT;
        break;
      case ENERGY:
        currentMode = POWER;
        break;
      case FREQUENCY:
        currentMode = ENERGY;
        break;
      case POWER_FACTOR:
        currentMode = FREQUENCY;
        break;
    }
  }

  updateDisplay(btnNxt, btnPrv);
  delay(100);  // Add a small delay to debounce the buttons
}

void updateDisplay(byte btnNxt, byte btnPrv) {
  lcd.setCursor(0, 2);
  lcd.print("                    ");

  lcd.setCursor(0, 2);
  switch (currentMode) {
    case VOLTAGE:
      lcd.print("Voltage: ");
      if (isVoltagePaused) {
        lcd.print(lastVoltage);
      } else {
        lastVoltage = pzem.voltage();
        lcd.print(lastVoltage);
        lcd.print("V");
      }
      break;
    case CURRENT:
      lcd.print("Current: ");
      if (isCurrentPaused) {
        lcd.print(lastCurrent);
      } else {
        lastCurrent = pzem.current();
        lcd.print(lastCurrent);
        lcd.print("A");
      }
      break;
    case POWER:
      lcd.print("Power: ");
      if (isPowerPaused) {
        lcd.print(lastPower);
      } else {
        lastPower = pzem.power();
        lcd.print(lastPower);
        lcd.print("W");
      }
      break;
    case ENERGY:
      lcd.print("Energy: ");
      if (isEnergyPaused) {
        lcd.print(lastEnergy);
      } else {
        lastEnergy = pzem.energy();
        lcd.print(lastEnergy);
        lcd.print("KWh");
      }
      break;
    case FREQUENCY:
      lcd.print("Frequency: ");
      if (isFrequencyPaused) {
        lcd.print(lastFrequency);
      } else {
        lastFrequency = pzem.frequency();
        lcd.print(lastFrequency);
        lcd.print("Hz");
      }
      break;
    case POWER_FACTOR:
      lcd.print("Power Factor: ");
      if (isPowerFactorPaused) {
        lcd.print(lastPowerFactor);
      } else {
        lastPowerFactor = pzem.pf();
        lcd.print(lastPowerFactor);
      }
      break;
  }

  lcd.setCursor(0, 3);
  lcd.print(btnNxt == LOW ? " NEXT" : (btnPrv == LOW ? " PREV" : ""));
}

void displayTime(unsigned long seconds) {
  unsigned long hours = seconds / 3600;
  unsigned long minutes = (seconds % 3600) / 60;
  unsigned long secs = seconds % 60;

  // Display time on LCD
  lcd.print(String(hours) + ":" + twoDigitString(minutes) + ":" + twoDigitString(secs));
}

String twoDigitString(unsigned long number) {
  if (number < 10) {
    return "0" + String(number);
  } else {
    return String(number);
  }
}
