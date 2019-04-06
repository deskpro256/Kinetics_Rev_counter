//=======================[INCLUDES]==========================
#include <avr/io.h>
#include "ssd1306.h"

#define NOP __asm__ __volatile__ ("nop\n\t")

//=======================[VARIABLES]==========================

unsigned int val = 0; //analog value from pin
unsigned int i = 0;
volatile int count = 0;
unsigned int t = 0;
unsigned int sens = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
static int interrupted;
char valChar[5] = {'0', '0', '0', '0'};
char countChar[5] = {'0', '0', '0', '0'};


//=======================[SETUP]==========================
void setup() {

  pinMode(0, OUTPUT);         // MOTOR  PA0
  pinMode(1, INPUT);          // START  PA1
  pinMode(2, INPUT);          // STOP   PA2
  pinMode(3, OUTPUT);         // BUZZER PA3
  pinMode(7, INPUT);          // POT    PA7
  pinMode(8, INPUT);          // SENSOR PB2

  interrupted = false;
  splashMusic();
  displaySetup();
  loop();
}

void NOPdelay(unsigned int duration) {
  for (unsigned int q = 0; q <= duration; q++) {
    NOP;
  }
}

//===========================[DISPLAY_SETUP]============================

void displaySetup() {
  ssd1306_128x64_i2c_init();
  ssd1306_fillScreen(0x00);
  ssd1306_setFixedFont(ssd1306xled_font6x8);

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
//===========================[SOFT_START]==========================
void softStart() {
  unsigned int on = 0;
  unsigned int off = 1000;
  while (on < 1000) {
    PORTA |= (1 << PA0);
    delay(on);
    PORTA &= !(1 << PA0);
    delay(off);
    on = on + 10;
    off = off - 10;
  }
}

//===============================[DONE]===============================
void done() {
  doneMusic();
  ssd1306_fillScreen(0x00);
  digitalWrite(0, LOW);

  while (digitalRead(1) != LOW) {
    ssd1306_printFixed (0, 0, "COUNTING DONE!", STYLE_BOLD);
    ssd1306_printFixed (0, 16, "REVOLUTIONS:", STYLE_BOLD);
    ssd1306_printFixedN (30, 32, valChar, STYLE_NORMAL, FONT_SIZE_2X);
  }
  if (digitalRead(1) == LOW) {
    ssd1306_fillScreen(0x00);
    setVal();
  }
}

//===========================[COUNTING]============================
void counting() {

  //TURN ON THE MOTOR "softly"
  softStart();
  PORTA |= (1 << PA0);

  while (count > 0) {

    if (digitalRead(8) == 0) {
      interrupted = true;
    }

    if (interrupted) {
      handleInterrupt();
      interrupted = false;
    }
    if (count >= 1000) {
      countChar[0] = ((count / 1000) + '0');                       //  tūkstoši  1
      countChar[1] = (((count % 1000) / 100) + '0');               //  simti     2
      countChar[2] = ((((count % 1000) % 100) / 10) + '0');        //  desmiti   3
      countChar[3] = (((((count % 1000) % 100) % 10) / 1) + '0');  //  vieni     4
    }
    else if (count < 1000) {
      countChar[0] = 0x20;      //show nothing
      countChar[1] = (((count % 1000) / 100) + '0');               //  simti     2
      countChar[2] = ((((count % 1000) % 100) / 10) + '0');        //  desmiti   3
      countChar[3] = (((((count % 1000) % 100) % 10) / 1) + '0');  //  vieni     4
    }
    else if (count < 100) {
      countChar[0] = 0x20;      //show nothing
      countChar[0] = 0x20;      //show nothing
      countChar[2] = ((((count % 1000) % 100) / 10) + '0');        //  desmiti   3
      countChar[3] = (((((count % 1000) % 100) % 10) / 1) + '0');  //  vieni     4
    }
    else if (count < 10) {
      countChar[0] = 0x20;      //show nothing
      countChar[0] = 0x20;      //show nothing
      countChar[0] = 0x20;      //show nothing
      countChar[3] = (((((count % 1000) % 100) % 10) / 1) + '0');  //  vieni     4
    }

    ssd1306_printFixedN (0, 0, "SET: ", STYLE_NORMAL, FONT_SIZE_2X);
    ssd1306_printFixedN (45, 0, valChar, STYLE_NORMAL, FONT_SIZE_2X);
    ssd1306_printFixedN (0, 20, "CURRENT: ", STYLE_NORMAL, FONT_SIZE_2X);
    ssd1306_printFixedN (30, 40, countChar, STYLE_NORMAL, FONT_SIZE_2X);

    //IF STOP BUTTON PRESSED, TURN OFF THE MOTOR, DISPLAY STOP
    if (digitalRead(2) == LOW) {
      PORTA &= !(1 << PA0);
      //digitalWrite(0, LOW);
      stopMusic();

      ssd1306_fillScreen(0x00);
      ssd1306_printFixedN (35, 0, "STOP!", STYLE_NORMAL, FONT_SIZE_2X);
      ssd1306_printFixedN (30, 30, "RESET!", STYLE_NORMAL, FONT_SIZE_2X);

      delay(1000);
      ssd1306_fillScreen(0x00);
      setVal();
    }
  }

  ssd1306_fillScreen(0x00);
  done();
}

//=======================[DISPLAY_SET_VALUE]==========================
void dispSetValue() {

  while (1) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 1) {
      previousMillis = currentMillis;
      val = analogRead(7);
      val = map(val, 0, 1021, 1, 50);
      val = val * 100;

      if (val < 999) {
        valChar[0] = 0x20;
        valChar[1] = ((val % 1000) / 100) + '0';
      }
      else {
        valChar[0] = (val / 1000) + '0';
        valChar[1] = ((val % 1000) / 100) + '0';
      }

      ssd1306_printFixedN (0, 0, "SET LIMIT:", STYLE_NORMAL, FONT_SIZE_2X);
      ssd1306_printFixedN (30, 30, valChar, STYLE_NORMAL, FONT_SIZE_2X);


      if (digitalRead(1) == LOW) {
        ssd1306_fillScreen(0x00);
        count = val;
        startMusic();
        counting();
      }
    }
  }
  ssd1306_fillScreen(0x00);
}

//=======================[SET_DISPLAY]==========================
void setVal() {

  ssd1306_fillScreen(0x00);
  ssd1306_printFixedN (10, 0, "SET YOUR", STYLE_NORMAL, FONT_SIZE_2X);
  ssd1306_printFixedN (0, 16, "REVOLUTION", STYLE_NORMAL, FONT_SIZE_2X);
  ssd1306_printFixedN (30, 32, "COUNT", STYLE_NORMAL, FONT_SIZE_2X);
  delay(2000);
  ssd1306_fillScreen(0x00);
  dispSetValue();
}

//=======================[SPLASH_SCREEN]==========================
void splashScreen() {
  ssd1306_fillScreen(0x00);
  ssd1306_printFixedN (12, 8, "KINETICS", STYLE_NORMAL, FONT_SIZE_2X);
  ssd1306_printFixed (4, 32, "R E V O L U T I O N", STYLE_NORMAL);
  ssd1306_printFixed (20, 48, "C O U N T E R", STYLE_NORMAL);
  delay(2000);
  setVal();
}
//=======================[LOOP]==========================
void loop() {
  splashScreen();
}
