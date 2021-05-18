//----------------------------------------------------------------------------------------------------------
//
//
//ARDUINO_Genius.ino - Genius(Simon) game implementantion on Arduino Uno
//
//
//Author: Leandro Barroso <lrbarroso84@hotmail.com>
//
//Version History:
//
//   v1.0 2021-05-14, Leandro Barroso
//       - initial version
//
//License: MIT
//
//----------------------------------------------------------------------------------------------------------

// ***ABREVIATIONS***
// FX:      effects
// Init:    initialize
// Prep:    prepare
// Reinit:  reinitiliaze
// Seq:     sequence

// ***LIBRARIES CONSTANTS***

//TONE PITCH
#define TONE_USE_INT
#define TONE_PITCH 432

//ARDUINO TONE PITCH LIBRARY - https://github.com/RodrigoDornelles/arduino-tone-pitch //
#include <Pitch.h>

// ***MAIN CONSTANTS***

// ---PINS CONSTANTS---

#define LED_GREEN 2
#define LED_YELLOW 3
#define LED_RED 4
#define LED_BLUE 5

#define BUTTON_GREEN 8
#define BUTTON_YELLOW 9
#define BUTTON_RED 10
#define BUTTON_BLUE 11

#define BUZZER 13

// ---MEASUREMENT UNITS CONSTANTS---

#define ONE_SECOND 1000
#define HALF_SECOND 500
#define QUARTER_SECOND 250

// ---GAME CONSTANTS---

#define SEQUENCE_LENGTH 6
#define UNDEFINED -1

// ***GLOBAL VARIABLES***

enum GameStates {
  READY_TO_NEXT_TURN,
  PLAYER_ASWERING,
  GAME_OVER_SUCESS,
  GAME_OVER_FAIL
};

int lightsSequence[SEQUENCE_LENGTH];
int currentTurn = 0;
int answersCount = 0;

// ***AUX FUNCTIONS***

// ---SETUP AUX FUNCTIONS---

void initPins() {
  // LIGHTS PINS
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // BUTTONS PINS
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);

  // OTHER PINS
  pinMode(BUZZER, OUTPUT);
}

int pickColor() {
  // RETURN A RANDOM NUMBER REFERRING TO A COLOR FROM LIGHTS PINS NUMBER RANGE   
  
  return random(LED_GREEN, LED_BLUE + 1);
}

void initGame() {

  // GETS A RANDOM NUMBER FROM ANALOGIC PORT 0 TO INITIALIZE RANDOM NUMBER GENERATOR
  int gameNumber = analogRead(0);
  randomSeed(gameNumber);

  // SORT RANDOM LIGHTS SEQUENCE
  for (int i = 0; i < SEQUENCE_LENGTH; i++) {
    lightsSequence[i] = pickColor();
  }
}


// ---LOOP AUX FUNCTIONS---

//GAME LOGIC FUNCTIONS

void prepNewTurn() {
  // INCREMENT TURN COUNTER, REINIT PLAYER'S ANSWERS COUNTER AND PLAY SEQUENCE'S LIGHTS/SOUNDS FX
  
  currentTurn++;
  answersCount = 0;
  if (currentTurn <= SEQUENCE_LENGTH) {
    blinkLightSequence();
  }
}

int checkPlayersAnswer() {
  // CHECK PRESSED BUTTON AND PLAY COLOR'S LIGHT/SOUND FX
  
  if (digitalRead(BUTTON_GREEN) == LOW) {
    return blinkLed(LED_GREEN);
  }
  if (digitalRead(BUTTON_YELLOW) == LOW) {
    return blinkLed(LED_YELLOW);
  }
  if (digitalRead(BUTTON_RED) == LOW) {
    return blinkLed(LED_RED);
  }
  if (digitalRead(BUTTON_BLUE) == LOW) {
    return blinkLed(LED_BLUE);
  }
  
  // RETURN UNDEFINED(-1) IF NO PRESSED BUTTON
  return UNDEFINED;
}

void processPlayersAnswer() {
  // Read and process pleyer's input on PLAYER_ASWERING state
   
  int answer = checkPlayersAnswer();

  //NO ANSWER//
  if (answer == UNDEFINED) {
    return;
  }
  //RIGHT ANSWER//
  if (answer == lightsSequence[answersCount]) {
    answersCount++;
  //WRONG ANSWER//
  } else {
    // IF WRONG ANSWER currentTurn IS SET TO A VALUE GREATER THAN SEQUENCE_LENGTH + 1
    Serial.println("Wrong sequence");
    currentTurn = SEQUENCE_LENGTH + 2;
  }
}

int currentState() {
  // BASED ON THE CURRENT TURN VALUE AND SEQ LENGTH COMPARISON RETURNS THE CURRENT STATE

  // IF THE CURRENT TURN IS LESS OR EQUALS SEQ LENGTH THE GAME IS RUNNING
  if (currentTurn <= SEQUENCE_LENGTH) {
    if (answersCount == currentTurn) {
      return READY_TO_NEXT_TURN;
    } else {
      return PLAYER_ASWERING;
    }
  // IF THE CURRENT TURN VALUE IS ESQUALS TO SEQ LENGTH + 1 PLAYER WINS
  } else if (currentTurn == SEQUENCE_LENGTH + 1) {
    return GAME_OVER_SUCESS;
  // OTHERWISE PLAYER LOSES
  } else {
    return GAME_OVER_FAIL;
  }
}

void gameOverSuccess() {
  blinkLed(LED_GREEN);
  blinkLed(LED_YELLOW);
  blinkLed(LED_RED);
  blinkLed(LED_BLUE);
  delay(250);
}

void gameOverFail() {
  playSound(300);
  digitalWrite(LED_GREEN,HIGH);
  digitalWrite(LED_YELLOW,HIGH);
  digitalWrite(LED_RED,HIGH);
  digitalWrite(LED_BLUE,HIGH);
  delay(ONE_SECOND);
  digitalWrite(LED_GREEN,LOW);
  digitalWrite(LED_YELLOW,LOW);
  digitalWrite(LED_RED,LOW);
  digitalWrite(LED_BLUE,LOW);
}

//LIGHT FX FUNCTIONS

int blinkLed(int ledPin) {
  // PLAYS COLOR'S SOUND EFFECT AND BLINKS ITS LED

  playLedSound(ledPin);
  
  digitalWrite(ledPin,HIGH);
  delay(HALF_SECOND);
  digitalWrite(ledPin,LOW);

  // RETURN THE LED'S PIN NUMBER
  return ledPin;
}

void blinkLightSequence() {
  for (int i = 0; i < currentTurn; i++) {
    blinkLed(lightsSequence[i]);
  }
}

//SOUND FX FUNCTIONS
void playSound(int frequence) {
  tone(BUZZER, frequence, QUARTER_SECOND);
}

void playLedSound(int ledPort) {
  // GETS LED'S PIN NUMBER AND PLAYS COLOR'S RELATIVE TUNE
  switch (ledPort) {
    case LED_GREEN:
      playSound(NOTE_E5);
      break;
    case LED_YELLOW:
      playSound(NOTE_CS4);
      break;
    case LED_RED:
      playSound(NOTE_A4);
      break;
    case LED_BLUE:
      playSound(NOTE_E4);
      break;
  }
}

// ***MAIN FUNCTIONS***

void setup() {
  Serial.begin(9600);
  initPins();
  initGame();
}

void loop() {
  Serial.print("CURRENT STATE: ");
  switch (currentState()) {
    case READY_TO_NEXT_TURN:
      Serial.println("READY_TO_NEXT_TURN");
      prepNewTurn();
      break;
    case PLAYER_ASWERING:
      Serial.println("PLAYER_ASWERING");
      processPlayersAnswer();
      break;
    case GAME_OVER_SUCESS:
      Serial.println("GAME_OVER_SUCESS");
      gameOverSuccess();
      break;
    case GAME_OVER_FAIL:
      Serial.println("GAME_OVER_FAIL");
      gameOverFail();
      break;
  }
  delay(HALF_SECOND);
}
