
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

// REFEREE ARDUINO

// SoftwareSerial to COMPUTER Arduino
#define rxPin 4
#define txPin 5
SoftwareSerial compSerial(rxPin, txPin);

// LCD pins
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Start Button
const int startButtonPin = 13;
int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Game State Variables
String stage = "stage0";
String prevMoves = "00000";   // first 5 moves
String winner = "noWinner";

String playerSelection = "?";
String computerSelection = "?";

char* cString;

// SETUP
void setup() {
  Serial.begin(9600);      // Player Arduino port
  compSerial.begin(9600);  // Computer Arduino port

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  pinMode(startButtonPin, INPUT);

  lcd.begin(16, 2);
  lcd.print("Referee Ready");
}

// MAIN LOOP
void loop() {

  // STAGE 0 — WAIT FOR START BUTTON
  if (stage == "stage0") {

    lcd.setCursor(0, 0);
    lcd.print("Press Start Btn ");

    int reading = digitalRead(startButtonPin);

    if (reading != lastButtonState) {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != buttonState) {
        buttonState = reading;

        if (buttonState == HIGH) {

          stage = "stage1";
          lcd.clear();
          lcd.print("Stage1: Mode Sel");

          // send stage1 to both Arduinos
          sendToBoth("stage1\n");
        }
      }
    }

    lastButtonState = reading;
  }

  // STAGE 1 — COMPUTER SELECTS MODE
  else if (stage == "stage1") {

    if (compSerial.available()) {
      String msg = compSerial.readStringUntil('\n');
      msg.trim();

      if (msg == "nextStage") {
        stage = "stage2";

        lcd.clear();
        lcd.print("Stage2: Moves");

        // Send stage2 + prevMoves to computer
        sendToComputer("stage2\n");
        sendToComputer(prevMoves + "\n");

        // Send "start" to player
        sendToPlayer("start\n");
      }
    }
  }

  // STAGE 2 — WAIT FOR MOVES
  else if (stage == "stage2") {

    if (Serial.available()) {

      String msg = Serial.readStringUntil('\n');
      msg.trim();

      // expect format "1:rock"
      int colonPos = msg.indexOf(':');
      if (colonPos > 0) {
        playerSelection = msg.substring(colonPos + 1);
        lcd.setCursor(0, 1);
        lcd.print("P:" + playerSelection + "   ");
      }
    }

    if (compSerial.available()) {

      String msg = compSerial.readStringUntil('\n');
      msg.trim();

      if (msg == "rock" || msg == "paper" || msg == "scissors") {
        computerSelection = msg;
        lcd.setCursor(8, 1);
        lcd.print("C:" + computerSelection + "   ");
      }
    }

    if (playerSelection != "?" && computerSelection != "?") {

      winner = evaluateWinner();

      updatePrevMoves(playerSelection);

      stage = "stage3";

      lcd.clear();
      lcd.print("Winner: ");
      lcd.print(winner);

      // send results to computer
      sendToComputer("stage3\n");
      sendToComputer(winner + "\n");

      // stop player
      sendToPlayer("stop\n");
    }
  }

  // STAGE 3 — SHOW RESULTS
  else if (stage == "stage3") {

    delay(3000);

    // goto stage4 to reset
    stage = "stage4";

    lcd.clear();
    lcd.print("Stage4: Reset");

    sendToComputer("stage4\n");
  }

  // STAGE 4 — RESET SYSTEM
  else if (stage == "stage4") {

    delay(2000);

    // reset everything
    playerSelection = "?";
    computerSelection = "?";
    winner = "noWinner";

    lcd.clear();
    lcd.print("Ready Next Game");

    stage = "stage0";
  }
}

// SEND HELPERS
void sendToPlayer(String s) {
  Serial.print(s);
}

void sendToComputer(String s) {
  compSerial.print(s);
}

void sendToBoth(String s) {
  Serial.print(s);
  compSerial.print(s);
}

// C STRING CONVERTER
void convert(String s) {
  if (cString != NULL) free(cString);
  cString = (char*)malloc(s.length() + 1);
  s.toCharArray(cString, s.length() + 1);
}

// WINNER EVALUATION
String evaluateWinner() {

  if (playerSelection == computerSelection)
    return "tie";

  if (playerSelection == "rock" && computerSelection == "scissors") return "player";
  if (playerSelection == "paper" && computerSelection == "rock")     return "player";
  if (playerSelection == "scissors" && computerSelection == "paper") return "player";

  return "computer";
}

// REVIOUS MOVES TRACKING
void updatePrevMoves(String move) {

  char c = '0';
  if (move == "rock") c = 'r';
  if (move == "paper") c = 'p';
  if (move == "scissors") c = 's';

  // shift left, append new move
  prevMoves = prevMoves.substring(1) + c;
}
