/*
  CS342 - Troy
  Final Project
  Group 64 - Rock, Paper, Scissors
  Anthony Youssef - ayous11
  Daniel Loera - dloer2
  Ryan Vincoy - rvinc
  June Hong - hhong43
  Abstract: The goal of this project is to create a Rock-Paper-Scissors game hosted on four 
  Arduinos that leverages a main system and multiple side systems to handle distributed gameplay. 
  The side Arduinos are responsible for capturing player inputs and transmitting them to the main Arduino, 
  which manages game logic, determines winners and losers, and communicates results back to each side system. 
  This design demonstrates effective serial communication and coordination between multiple microcontrollers, 
  emphasizing modular design, synchronization, and real-time data exchange. The project highlights how 
  interconnected embedded systems can work together to perform a unified task efficiently and reliably.
*/

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
const unsigned long debounceDelay = 50;

// Game state variables
String stage = "stage0";
String prevMoves = "00000";
String winner = "noWinner";

String playerSelection = "?";
String computerSelection = "?";

<<<<<<< HEAD
// Non-blocking timers
unsigned long stageTimer = 0;
unsigned long stageTimeout = 0;


=======
int playerScore = 0;
int computerScore = 0;

char* cString;
>>>>>>> refs/remotes/origin/aydev

void setup() {
  Serial.begin(9600);      // Player Arduino
  compSerial.begin(9600);  // Computer Arduino

  pinMode(startButtonPin, INPUT);

  lcd.begin(16, 2);
  lcd.print("Referee Ready");
}



void loop() {

  // STAGE 0 — WAIT FOR START BUTTON
  if (stage == "stage0") {

    lcd.setCursor(0, 0);
    lcd.print("Press Start Btn ");

    int reading = digitalRead(startButtonPin);

    if (reading != lastButtonState) lastDebounceTime = millis();

    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != buttonState) {
        buttonState = reading;

        if (buttonState == HIGH) {
          stage = "stage1";
          lcd.clear();
          lcd.print("Stage1: Mode Sel");

          // Tell both Arduinos
          Serial.print("stage1\n");     // to PLAYER
          compSerial.print("stage1\n"); // to COMPUTER
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

        // Send stage2 and previous move memory to the computer
        compSerial.print("stage2\n");
        compSerial.print(prevMoves + "\n");

        // Tell player to begin input
        Serial.print("start\n");
      }
    }
  }



  // =====================================================
  //  STAGE 2 — WAIT FOR PLAYER + COMPUTER MOVES
  // =====================================================
  else if (stage == "stage2") {

    // PLAYER MOVE
    if (Serial.available()) {
      String msg = Serial.readStringUntil('\n');
      msg.trim();

      if (msg == "rock" || msg == "paper" || msg == "scissors") {
        playerSelection = msg;
        lcd.setCursor(0, 1);
        lcd.print("P:" + playerSelection + "   ");
      }
    }

    // COMPUTER MOVE
    if (compSerial.available()) {
      String msg = compSerial.readStringUntil('\n');
      msg.trim();

      if (msg == "rock" || msg == "paper" || msg == "scissors") {
        computerSelection = msg;
        lcd.setCursor(8, 1);
        lcd.print("C:" + computerSelection + "   ");
      }
    }

    // Once both moves are received → go to stage3
    if (playerSelection != "?" && computerSelection != "?") {

      winner = evaluateWinner();
      updatePrevMoves(playerSelection);

      stage = "stage3";
      stageTimer = millis();  // start timer
      lcd.clear();
      lcd.print("Winner: ");
      lcd.print(winner);

      // Tell computer Arduino who won
      compSerial.print("stage3\n");
      compSerial.print(winner + "\n");

      // Tell player Arduino to stop reading buttons
      Serial.print("stop\n");
    }
  }



  // =====================================================
  //  STAGE 3 — DISPLAY WINNER (NON-BLOCKING)
  // =====================================================
  else if (stage == "stage3") {

    if (millis() - stageTimer >= 2500) { // 2.5 seconds
      stage = "stage4";

      lcd.clear();
      lcd.print("Resetting...");

      compSerial.print("stage4\n");
      stageTimer = millis();
    }
  }



  // =====================================================
  //  STAGE 4 — RESET GAME (NON-BLOCKING)
  // =====================================================
  else if (stage == "stage4") {

    if (millis() - stageTimer >= 1500) { // 1.5 seconds

      // Reset variables
      playerSelection = "?";
      computerSelection = "?";
      winner = "noWinner";

      lcd.clear();
      lcd.print("Press Start Btn");

      stage = "stage0";
    }
  }
}



// =====================================================
//  HELPER FUNCTIONS
// =====================================================

String evaluateWinner() {

  if (playerSelection == computerSelection)
    return "tie";

  if (playerSelection == "rock" && computerSelection == "scissors") return "player";
  if (playerSelection == "paper" && computerSelection == "rock")     return "player";
  if (playerSelection == "scissors" && computerSelection == "paper") return "player";

  return "computer";
}


void updatePrevMoves(String move) {

  char code = '0';
  if (move == "rock")     code = 'r';
  if (move == "paper")    code = 'p';
  if (move == "scissors") code = 's';

  prevMoves = prevMoves.substring(1) + code;
}

void prev(String s) {
  prevMoves += s[0];
}
