#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;

// STM32 Blue Pill pins
const int SENSOR_PIN = PA0;
const int MODE_PIN   = PB12;  // jumper wire: unplugged = demo, to GND = real
const int GREEN_LED  = PB0;
const int YELLOW_LED = PB1;
const int RED_LED    = PB10;
const int BUZZER     = PB11;

bool demoMode = true;
int demoLevel = 0;
unsigned long lastDemoChange = 0;
unsigned long lastBlink = 0;
unsigned long diffSince = 0;
bool redState = false;
bool buzzerOn = false;

void buzzerOff() {
  noTone(BUZZER);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  buzzerOn = false;
}

void allOff() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  buzzerOff();
}

void checkModeWire() {
  bool wantDemo = (digitalRead(MODE_PIN) == HIGH);

  if (wantDemo == demoMode) {   // nothing changed
    diffSince = 0;
    return;
  }

  if (diffSince == 0) diffSince = millis();

  if (millis() - diffSince > 100) {   // stable for 100 ms, so accept it
    demoMode = wantDemo;
    diffSince = 0;
    demoLevel = 0;
    lastDemoChange = millis();
    allOff();
    lcd.clear();
  }
}

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(MODE_PIN, INPUT_PULLUP);

  analogReadResolution(12);

  demoMode = (digitalRead(MODE_PIN) == HIGH);   // match the wire at startup

  Wire.setSCL(PB6);
  Wire.setSDA(PB7);
  Wire.begin();

  int status = lcd.begin(16, 2);
  lcd.backlight();                              // always call, even if status != 0
  if (status == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Air Quality");
    lcd.setCursor(0, 1);
    lcd.print("Starting...");
  }

  delay(1500);
  lcd.clear();
}

void loop() {
  checkModeWire();

  int sensorValue;

  if (demoMode) {
    if (millis() - lastDemoChange >= 5000) {
      lastDemoChange = millis();
      demoLevel = (demoLevel + 1) % 3;
    }

    if (demoLevel == 0) sensorValue = 500;
    else if (demoLevel == 1) sensorValue = 2000;
    else sensorValue = 3500;
  } else {
    sensorValue = analogRead(SENSOR_PIN);
  }

  const char* level;

  if (sensorValue < 1365) {
    level = "LOW";
    allOff();
    digitalWrite(GREEN_LED, HIGH);
  } else if (sensorValue < 2730) {
    level = "MODERATE";
    allOff();
    digitalWrite(YELLOW_LED, HIGH);
  } else {
    level = "HIGH";
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);

    if (millis() - lastBlink >= 500) {
      lastBlink = millis();
      redState = !redState;
      digitalWrite(RED_LED, redState);
    }

    if (!buzzerOn) {
      tone(BUZZER, 1000);
      buzzerOn = true;
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("Air: ");
  lcd.print(level);
  lcd.print("           ");

  lcd.setCursor(0, 1);
  lcd.print("Response:");
  lcd.print(sensorValue);
  lcd.print("   ");

  lcd.setCursor(13, 1);          // debug: raw pin reading (1 = open, 0 = grounded)
  lcd.print(digitalRead(MODE_PIN));

  lcd.setCursor(15, 1);          // D = demo, R = real
  lcd.print(demoMode ? 'D' : 'R');

  delay(50);
}