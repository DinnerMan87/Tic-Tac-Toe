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

#include <SoftwareSerial.h>
SoftwareSerial MASTER(12,11);

//LEDS
const int rockLED = 5, paperLED = 6, scissorsLED = 7;
const int RLED = 9, GLED = 10, BLED = A1;

//Buttons
const int rockButton = 2, paperButton = 3, scissorsButton = 4;

//Other Devices
const int pMeter = A0, buzzer = 8;

//MISC
String choice = "none\n";
String stage = "?";
bool win = false;
int winCycle = 0; //It stops the rgb and buzzer after a certain number

void setup() {
  pinMode(rockLED,     OUTPUT);
  pinMode(paperLED,    OUTPUT);
  pinMode(scissorsLED, OUTPUT);
  
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(BLED, OUTPUT);
  
  pinMode(rockButton,     INPUT);
  pinMode(paperButton,    INPUT);
  pinMode(scissorsButton, INPUT);
  
  pinMode(pMeter, INPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);
  MASTER.begin(9600);

  Serial.print("\n\n\nACTIVE\n");
}

int oldMill = 0;
void loop() {
  int newMill = millis();

  if (MASTER.available() > 0) {
    String read = MASTER.readStringUntil('\n');
    Serial.println("PING: " + read);

     if (read == "stage2") {
      stage = "2";
      Serial.println("Stage 2");
      
      win = false;
      noTone(buzzer);

    } else if (read == "stage3") {
      stage = "3";
      //Serial.print("Stage 3: sending player choice - ");
      //MASTER.write(choice.c_str());
        if (!win){
        noTone(buzzer);
        winCycle = 0;
        analogWrite(RLED, 0);
        analogWrite(GLED, 0);
        analogWrite(BLED, 0);
        digitalWrite(rockLED, LOW);
        digitalWrite(paperLED, LOW);
        digitalWrite(scissorsLED, LOW);}
    
    } else if (read == "win"){
      win = true;
      Serial.println("WON");
    }
    else if (read == "lose"){
        stage = "0";
        win = false;
        noTone(buzzer);
        winCycle = 0;
        analogWrite(RLED, 0);
        analogWrite(GLED, 0);
        analogWrite(BLED, 0);
        digitalWrite(rockLED, LOW);
        digitalWrite(paperLED, LOW);
        digitalWrite(scissorsLED, LOW);
    }
  }

  if (stage == "2"){
    
    if (digitalRead(rockButton) == 1){
      choice = "rock\n";
      digitalWrite(rockLED, HIGH);

      digitalWrite(paperLED, LOW);
      digitalWrite(scissorsLED, LOW);
      
      MASTER.print("rock\n");
    }else if (digitalRead(paperButton) == 1){
      choice = "paper\n";
      digitalWrite(paperLED, HIGH);

      digitalWrite(rockLED, LOW);
      digitalWrite(scissorsLED, LOW);

      MASTER.write("paper\n");
    }else if (digitalRead(scissorsButton) == 1){
      choice = "scissors\n";
      digitalWrite(scissorsLED, HIGH);

      digitalWrite(rockLED, LOW);
      digitalWrite(paperLED, LOW);

      MASTER.write("scissors\n");
    } 

  }

  if (newMill - oldMill > 100){
    oldMill = newMill;
    if (win){
      winCycle++;

      analogWrite(RLED, random(255));
      analogWrite(GLED, random(255));
      analogWrite(BLED, random(255));

      int a = analogRead(pMeter);

      if (a > 50)
        tone(buzzer, a);
      else
        noTone(buzzer);

      if (winCycle >= 20){
        stage = "0";
        win = false;
        noTone(buzzer);
        winCycle = 0;
        analogWrite(RLED, 0);
        analogWrite(GLED, 0);
        analogWrite(BLED, 0);
        digitalWrite(rockLED, LOW);
        digitalWrite(paperLED, LOW);
        digitalWrite(scissorsLED, LOW);
      }
    }
  }
}
