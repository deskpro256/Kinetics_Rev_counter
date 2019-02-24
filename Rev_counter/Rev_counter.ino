//=======================[INCLUDES]==========================
#include <avr/io.h>
#include <Tiny4kOLED.h>
#include <TinyWireM.h>

#define NOP __asm__ __volatile__ ("nop\n\t")


//=======================[VARIABLES]==========================

unsigned val = 0; //analog value from pin
unsigned i = 0;
volatile int count = 0;
unsigned t = 0;
unsigned sens = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
static int interrupted;


//=======================[SETUP]==========================
void setup() {

  pinMode(0, OUTPUT);         // MOTOR  PA0
  pinMode(1, INPUT_PULLUP);   // START  PA1
  pinMode(2, INPUT_PULLUP);   // STOP   PA2
  pinMode(3, OUTPUT);         // BUZZER PA3
  pinMode(7, INPUT);          // POT    PA7
  pinMode(8, INPUT);          // SENSOR PB2

  interrupted = false;
  oled.begin();
  oled.on();
  oled.clear();
  //sei();
  splashMusic();
  loop();
}

void NOPdelay(unsigned int duration) {
  for (unsigned int q = 0; q <= duration; q++) {
    NOP;
  }
}

//===========================[INTERRUPT]============================

void handleInterrupt() {
  count = count - 1;
  interrupted = false;
}

//===========================[BEEP]============================

void beep(unsigned int onTime, unsigned int offTime, unsigned int noteCount) {
  for (unsigned int i = 0; i <= noteCount; i++) {
    PORTA |= (1 << PA3);
    NOPdelay(onTime * 160000);
    PORTA &= !(1 << PA3);
    NOPdelay(offTime * 160000);
  }
}

//===========================[DONE_MUSIC]============================
void doneMusic() {
  beep(2, 1, 8);
}
//===========================[START_MUSIC]===========================
void startMusic() {
  beep(3, 2, 3);
}
//===========================[STOP_MUSIC]============================
void stopMusic() {
  beep(2, 2, 3);
}
//===========================[SPLASH_MUSIC]==========================
void splashMusic() {
  beep(2, 1, 5);
}

//===============================[DONE]===============================
void done() {
  doneMusic();
  oled.setFont(FONT6X8);
  digitalWrite(0, LOW);
  oled.setCursor(0, 0);
  oled.print(F("COUNTING DONE!    "));
  oled.setCursor(0, 1);
  oled.print(F(" REVS:           "));
  oled.setFont(FONT8X16);
  oled.setCursor(40, 2);
  oled.print(val);
  oled.print(F("                   "));
  if (digitalRead(1) == LOW) {
    setVal();
  }
}

//===========================[COUNTING]============================
void counting() {

  oled.setFont(FONT6X8);

  //TURN ON THE MOTOR
  digitalWrite(0, HIGH);

  while (count > 0) {

    if (digitalRead(8) == 0) {
      interrupted = true;
    }

    if (interrupted) {
      handleInterrupt();
      interrupted = false;
    }
    
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

    //IF STOP BUTTON PRESSED, TURN OFF THE MOTOR, DISPLAY STOP
    if (digitalRead(2) == LOW) {
      digitalWrite(0, LOW);
      oled.setFont(FONT8X16);
      stopMusic();
      oled.clear();
      oled.setCursor(38, 0);
      oled.print(F("S T O P!"));
      oled.setCursor(25, 2);
      oled.print(F("R E S E T!"));
      delay(1000);
      setVal();
    }
  }
  oled.clear();
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
      if (digitalRead(1) == LOW) {
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

//=======================[SPLASH_SCREEN]==========================
void splashScreen() {
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
//=======================[LOOP]==========================
void loop() {
  splashScreen();
}
