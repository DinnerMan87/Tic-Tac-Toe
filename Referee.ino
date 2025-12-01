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

// ------------------------------------------------------------
//  REFEREE ARDUINO
// ------------------------------------------------------------

// Player Arduino (pins 3 = RX, 2 = TX)
#define rxPinP 3
#define txPinP 2
SoftwareSerial playerSerial(rxPinP, txPinP);

// Computer Arduino (pins 4 = RX, 5 = TX)
#define rxPinC 4
#define txPinC 5
SoftwareSerial compSerial(rxPinC, txPinC);

// LCD pins (RW is tied to GND)
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

// Start button — external pull-down resistor
const int startButtonPin = 6;

// Debounce variables
int buttonState = LOW;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Game State
String stage = "stage0";
String prevMoves = "00000";
String winner = "noWinner";
String playerSelection = "?";
String computerSelection = "?";

// Timers
unsigned long stageTimer = 0;

// Debug tracker
String lastDebugStage = "";
unsigned long stageDebugTimer = 0;
const unsigned long stageDebugInterval = 1500;

// ------------------------------------------------------------
// DEBUG: print stage transitions + periodic updates
// ------------------------------------------------------------
void debugStage() {
  if (stage != lastDebugStage) {
    Serial.print("DEBUG: Stage changed to: ");
    Serial.println(stage);
    lastDebugStage = stage;
  }

  if (millis() - stageDebugTimer >= stageDebugInterval) {
    stageDebugTimer = millis();
    Serial.print("DEBUG: Still in stage: ");
    Serial.println(stage);
  }
}

// ------------------------------------------------------------
// SETUP
// ------------------------------------------------------------
void setup() {
  Serial.begin(9600);        // Debug output to Serial Monitor
  playerSerial.begin(9600);  // Player Arduino
  compSerial.begin(9600);    // Computer Arduino

  pinMode(startButtonPin, INPUT); // external pull-down

  lcd.begin(16, 2);
  lcd.print("Referee Ready");

  Serial.println("DEBUG: Boot complete, stage0");
}

// ------------------------------------------------------------
// MAIN LOOP
// ------------------------------------------------------------
void loop() {

  // ============================================================
  //  STAGE 0 — WAIT FOR START BUTTON
  // ============================================================
  if (stage == "stage0") {

    lcd.setCursor(0, 0);
    lcd.print("Press Start Btn ");

    int reading = digitalRead(startButtonPin);

    if (reading != lastButtonState)
      lastDebounceTime = millis();

    if ((millis() - lastDebounceTime) > debounceDelay) {

      if (reading != buttonState) {
        buttonState = reading;

        if (buttonState == HIGH) {
          Serial.println("DEBUG: Button PRESSED!");

          lcd.clear();
          lcd.print("Game Starting...");

          stageTimer = millis();
          stage = "startingMessage";
        }
      }
    }

    lastButtonState = reading;
  }

  // ============================================================
  // STARTING MESSAGE — 1 second screen
  // ============================================================
  else if (stage == "startingMessage") {

    if (millis() - stageTimer >= 5000) {

      stage = "stage1";
      lcd.clear();
      lcd.print("Stage1: Mode Sel");

      Serial.println("DEBUG: Moving to Stage 1");

      // Tell both Arduinos to enter stage1
      playerSerial.print("stage1\n");
      compSerial.print("stage1\n");
    }
  }

  // ============================================================
  //  STAGE 1 — COMPUTER SELECTS THINK / RANDOM
  // ============================================================
  else if (stage == "stage1") {

    compSerial.listen();
    if (compSerial.available()) {

      String msg = compSerial.readStringUntil('\n');
      msg.trim();

      Serial.print("DEBUG: Received from computer: ");
      Serial.println(msg);

      if (msg == "nextStage") {

        stage = "stage2";
        lcd.clear();
        lcd.print("Stage2: Moves");
        Serial.println("DEBUG: Moving to Stage 2");

        // Give computer previous moves pattern
        compSerial.print("stage2\n");

        // Tell player to show selections
        playerSerial.print("stage2\n");
      }
    }
  }

// ============================================================
//  STAGE 2 — WAIT FOR BOTH MOVES
// ============================================================
else if (stage == "stage2") {

    // PLAYER MOVE
    playerSerial.listen();
    if (playerSerial.available()) {
        String msg = playerSerial.readStringUntil('\n');
        msg.trim();

        Serial.print("DEBUG: Player sent -> ");
        Serial.println(msg);

        if (msg == "rock" || msg == "paper" || msg == "scissors") {
            playerSelection = msg;
            lcd.setCursor(0, 1);
            lcd.print("P:" + playerSelection + "   ");
        }
    }

    // COMPUTER MOVE (Randomly chosen by Referee)
    if (computerSelection == "?") { // only generate once
        int r = random(1, 4); // 1 = rock, 2 = paper, 3 = scissors
        if (r == 1) computerSelection = "rock";
        else if (r == 2) computerSelection = "paper";
        else computerSelection = "scissors";

        lcd.setCursor(8, 1);
        lcd.print("C:" + computerSelection + "   ");
        Serial.print("DEBUG: Computer chose -> ");
        Serial.println(computerSelection);
    }

    // If both moves received → determine winner
    if (playerSelection != "?" && computerSelection != "?") {

        winner = evaluateWinner();
        updatePrevMoves(playerSelection);

        stage = "stage3";
        stageTimer = millis();

        lcd.clear();
        lcd.print("Winner: ");
        lcd.print(winner);

        // notify player only (no need to notify computer since it's internal)
        playerSerial.print("stage3\n");
        compSerial.print("stage3\n");
    }
}


  // ============================================================
  //  STAGE 3 — SHOW WINNER FOR 2.5 SEC
  // ============================================================
  else if (stage == "stage3") {
    if (millis() - stageTimer >= 5000) {

      stage = "stage4";
      lcd.clear();
      lcd.print("Resetting...");

      compSerial.print("stage4\n");

      stageTimer = millis();
    }
  }

  // ============================================================
  //  STAGE 4 — RESET GAME
  // ============================================================
  else if (stage == "stage4") {

    if (millis() - stageTimer >= 1500) {

      playerSelection = "?";
      computerSelection = "?";
      winner = "noWinner";

      lcd.clear();
      lcd.print("Press Start Btn");

      stage = "stage0";
    }
  }

  // Always print debugging stage info
  debugStage();
}

// ------------------------------------------------------------
// HELPER FUNCTIONS
// ------------------------------------------------------------

String evaluateWinner() {

  if (playerSelection == computerSelection)
    return "tie";

  if (playerSelection == "rock"     && computerSelection == "scissors"){playerSerial.write("win\n"); return "win\n";}
  if (playerSelection == "paper"    && computerSelection == "rock"){ playerSerial.write("win\n");     return "win\n";}
  if (playerSelection == "scissors" && computerSelection == "paper"){ playerSerial.write("win\n");   return "win\n";}
  if (playerSelection == computerSelection)
  return "tie";

  playerSerial.print("lose\n");
  
  return "Computer\n";
  
}

void updatePrevMoves(String move) {
  char code = '0';
  if (move == "rock")     code = 'r';
  if (move == "paper")    code = 'p';
  if (move == "scissors") code = 's';
  prevMoves = prevMoves.substring(1) + code;
}
