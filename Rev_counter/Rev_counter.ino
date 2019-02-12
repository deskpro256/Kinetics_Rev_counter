//=======================[INCLUDES]==========================
#include <avr/io.h>
#include <Tiny4kOLED.h>
#include <TinyWireM.h>

//=======================[VARIABLES]==========================

int val = 0; //analog value from pin
int i = 0;
volatile int count = 0;
int t = 0;
int sens = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 1000;


//=======================[SETUP]==========================
void setup() {
  oled.begin();
  oled.on();
  oled.clear();

  pinMode(0, OUTPUT);         // MOTOR
  pinMode(3, OUTPUT);         // BUZZER
  pinMode(1, INPUT_PULLUP);   // START
  pinMode(2, INPUT_PULLUP);   // STOP
  pinMode(7, INPUT);          // POT
  pinMode(8, INPUT);          // SENSOR
  attachInterrupt(8, ISR_thing(), FALLING); //GIMSK |= 0b000000001  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  loop();
}

//===========================[INTERRUPT]============================

void ISR_thing() {
  count--;
}

//===========================[START_MUSIC]============================

void startMusic() {
  tone(0, HIGH);
  delay(100);
  tone(0, LOW);
}

//===========================[STOP_MUSIC]============================
void stopMusic() {
  tone(0, HIGH);
  delay(300);
  tone(0, LOW);
}

//===============================[DONE]===============================
void done() {
  oled.setFont(FONT6X8);
  digitalWrite(3, LOW);
  oled.setCursor(0, 0);
  oled.print(F("COUNTING DONE!    "));
  oled.setCursor(0, 1);
  oled.print(F(" REVS:           "));
  oled.setFont(FONT8X16);
  oled.setCursor(40, 2);
  oled.print(val);
  oled.print(F("                   "));
  if (digitalRead(1) == HIGH) {
    setVal;
  }
}

//===========================[COUNTING]============================
void counting() {

  oled.setFont(FONT6X8);

  digitalWrite(3, HIGH);

  while (count > 0) {

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 5) {
      previousMillis = currentMillis;

      oled.setFont(FONT6X8);
      oled.setCursor(0, 0);
      oled.print(F("S E T :    "));
      oled.print(val);
      oled.print(F("                             "));
      oled.setCursor(0, 1);
      oled.print(F("C U R R E N T:                          "));
      oled.setFont(FONT8X16);
      oled.setCursor(0, 2);
      oled.print(F("                "));
      oled.setCursor(40, 2);
      oled.print(count);

      if (digitalRead(2) == HIGH) {
        oled.setFont(FONT8X16);
        stopMusic();
        oled.clear();
        digitalWrite(3, LOW);
        oled.setCursor(40, 0);
        oled.print(F("S T O P!"));
        oled.setCursor(25, 2);
        oled.print(F("R E S E T!"));
        delay(1000);
        setVal();
      }
    }
  }
  oled.clear();
  stopMusic();
  done();
}
//=======================[DISPLAY_SET_VALUE]==========================
void dispSetValue() {
  oled.setFont(FONT8X16);
  while (1) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 1) {
      previousMillis = currentMillis;
      val = analogRead(7);
      val = map(val, 0, 1021, 1, 50);
      val = val * 100;
      oled.setCursor(0, 0);
      oled.print(F("S E T  L I M I T"));
      oled.setCursor(0, 2);
      oled.print(F("                   "));
      oled.setCursor(40, 2);
      oled.print(val);
      if (digitalRead(1) == 1) {
        oled.clear();
        count = val;
        startMusic();
        oled.setFont(FONT6X8);
        counting();
      }
    }
  }
  oled.clear();
}

//=======================[SET_DISPLAY]==========================
void setVal() {
  oled.clear();
  oled.setFont(FONT6X8);
  oled.setCursor(10, 1);
  oled.print(F(" S E T  Y O U R"));
  oled.setCursor(2, 2);
  oled.print(F("R E V O L U T I O N"));
  oled.setCursor(20, 3);
  oled.print(F("  C O U N T"));
  delay(2000);
  oled.clear();
  dispSetValue();
}
//=======================[LOOP]==========================
void loop() {
  oled.clear();
  oled.setFont(FONT8X16);
  oled.setCursor(3, 0);
  oled.print(F("K I N E T I C S"));
  oled.setFont(FONT6X8);
  oled.setCursor(30, 2);
  oled.print(F("REVOLUTION"));
  oled.setCursor(35, 3);
  oled.print(F(" COUNTER"));
  delay(2000);
  oled.setFont(FONT6X8);
  setVal();
}
