#include <Servo.h>

//Computer Arduino Code
//11-29-2025 at 6:02 PM couldn't get servo motor to work

unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
const long interval = 10000; //ten seconds

const int buttonPinThink = 2; //Thinking Button Mode next to RGB LED
int buttonStateThink;
int lastButtonStateThink = LOW;
unsigned long lastDebounceTimeThink = 0;
unsigned long debounceDelayThink = 50;

const int buttonPinRandom = 3; //Random Button Mode next to buzzer
int buttonStateRandom;
int lastButtonStateRandom = LOW;
unsigned long lastDebounceTimeRandom = 0;
unsigned long debounceDelayRandom = 50;

int ledR = 7; //rock led = yellow
int ledP = 6; //paper led = green
int ledS = 5; //scissors led = blue
int ledStateR = LOW;
int ledStateP = LOW;
int ledStateS = LOW;

const int buzzerPin = 4; //pin for buzzer
long int hz = 1000; //frequency for buzzer

const int potentPin = A0; //analog pin for the potentiometer

const int redPin = 12; //pin for red on the RBG LED
const int greenPin = 11; //pin for green on the RGB LED
const int bluePin = 8; //pin for blue on the RBG LED

Servo servo1; //set up instance of Servo motor
int servoPin = 13; //pin for Servo motor

//String gameSelection = ""; //data sent from Referee of whether its two player or player vs computer
                       //0=nothing selected, 1=two players, 2=player vs think
String stage = "stage0"; //data sent from Referee of stage the game is on
String numOfRounds = "round0"; //data sent from Referee of rounds played so far
String prevMoves; //data sent from Referee of previous moves player made
                   //0=no moves played, r=rock. p=paper, s=scissors
String winner = "noWinner"; //data sent from Referee of who the winner is
                //0=no winner, 1=playerOne, 2=playerTwo, 3=computer

int computerMode = 0; //player selection of making computer "think" or play randomly
                      //0=nothing selected, 1=random, 2=thinking
String computerSelection = "?"; //data sent to Referee of the choice computer makes
                           //0=no selection, 1=rock, 2=paper, 3=scissors

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPinRandom, INPUT);
  pinMode(buttonPinThink, INPUT);
  pinMode(ledR, OUTPUT);
  pinMode(ledP, OUTPUT);
  pinMode(ledS, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  servo1.attach(servoPin);
  randomSeed(analogRead(0));
}

void loop() {
  // put your main code here, to run repeatedly:
  hz = analogRead(potentPin);
  //tests for hardware
  //tone(buzzerPin, hz);
  //digitalWrite(ledR, HIGH);
  //digitalWrite(ledP, HIGH);
  //digitalWrite(ledS, HIGH);
  //int readingR = digitalRead(buttonPinRandom);
  //Serial.println(readingR);
  //int readingT = digitalRead(buttonPinThink);
  //Serial.println(readingT);
  //analogWrite(redPin, 180);
  //analogWrite(greenPin, 0);
  //analogWrite(bluePin , 0);
  //servo1.write(0);
  //delay(1000);
  //servo1.write(90);
  //delay(1000);
  //servo1.write(180);
  //delay(1000);
  //lightshow();
  if (stage == "stage0") {
    noTone(buzzerPin);
    if (Serial.available() > 0) {
      stage = Serial.readStringUntil('\n');
    }
  } else if (stage == "stage1") {
    if (Serial.available() > 0) {
      stage = Serial.readStringUntil('\n');
    }
    int readingThink = digitalRead(buttonPinThink);
    if (readingThink != lastButtonStateThink){
      lastDebounceTimeThink = millis();
    }

    int readingRandom = digitalRead(buttonPinRandom);
    if (readingRandom != lastButtonStateRandom){
      lastDebounceTimeRandom = millis();
    }

    if ((millis() - lastDebounceTimeThink) > debounceDelayThink) {
      if (readingThink != buttonStateThink) {
        buttonStateThink = readingThink;
        if(buttonStateThink == HIGH) {
          computerMode = 2; //computerMode is thinking
          Serial.write("nextStage\n");
        }
      }
    }
    lastButtonStateThink = readingThink;

    if ((millis() - lastDebounceTimeRandom) > debounceDelayRandom) {
      if (readingRandom != buttonStateRandom) {
        buttonStateRandom = readingRandom;
        if(buttonStateRandom == HIGH) {
          computerMode = 1; //computerMode is random
          Serial.write("nextStage\n");
        }
      }
    }
    lastButtonStateRandom = readingRandom;
  } else if (stage == "stage2") {
    if (Serial.available() > 0) {
      stage = Serial.readStringUntil('\n');
      prevMoves = Serial.readStringUntil('\n');
    }
    if(computerMode == 1 || prevFunc()) { //randomly chosen

      int selection = random(1,4); //1=rock,2=paper,3=scissors
      select(selection);
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        tone(buzzerPin, hz);
        char* cString = (char*) malloc(sizeof(char)*(computerSelection.length()+1));
        computerSelection.toCharArray(cString, computerSelection.length()+1);
        Serial.write(cString);
        digitalWrite(ledR, ledStateR);
        digitalWrite(ledP, ledStateP);
        digitalWrite(ledS, ledStateS);
      }

    } else if (computerMode == 2) {
      int r = 0;
      int p = 0;
      int s = 0;
      int total = 0;
      for(int i = 0; i < prevMoves.length(); i++) {
        if(prevMoves[i] != '0') {
          total++;
          if (prevMoves[i] == 'r') {
            r++;
          } else if (prevMoves[i] == 'p') {
            p++;
          } else {
            s++;
          }
        }
      }
      int selection = think(r,p,s,total);
      select(selection);
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        tone(buzzerPin, hz);
        char* cString = (char*) malloc(sizeof(char)*(computerSelection.length()+1));
        computerSelection.toCharArray(cString, computerSelection.length()+1);
        Serial.write(cString);
        digitalWrite(ledR, ledStateR);
        digitalWrite(ledP, ledStateP);
        digitalWrite(ledS, ledStateS);
      }

    }
  } else if (stage == "stage3") {
    noTone(buzzerPin);
    if (Serial.available() > 0) {
      stage = Serial.readStringUntil('\n');
      winner = Serial.readStringUntil('\n');
    }
    if (winner == "computer") {
      lightshow();
    }
  } else if (stage == "stage4") {
    digitalWrite(ledR, LOW);
    digitalWrite(ledP, LOW);
    digitalWrite(ledS, LOW);
    if (Serial.available() > 0) {
      stage = Serial.readStringUntil('\n');
      winner = Serial.readStringUntil('\n');
    }
    if (winner = "computer") {
      lightshow();
      tone(buzzerPin, hz);
    }
  }
}

bool prevFunc() { //returns whether prevMoves has 5 moves in it
  return prevMoves[0] == '0' &&  prevMoves[1] == '0' &&  prevMoves[2] == '0' &&  prevMoves[3] == '0' &&  prevMoves[4] == '0';
}

int think(int r, int p, int s, int total) {
  int a[total];
  for (int i = 0; i < total; i++) {
    if (i < r) {
      a[i] = 1;
    } else if (i-r < p) {
      a[i] = 2;
    } else {
      a[i] = 3;
    }
  }
  return a[random(0,total)];
}

void select(int selection) {
  if (selection == 1) { //rock
        ledStateR = HIGH;
        ledStateP = LOW;
        ledStateS = LOW;
        computerSelection = "rock\n";
  } else if (selection == 2){ //paper
        ledStateR = LOW;
        ledStateP = HIGH;
        ledStateS = LOW;
        computerSelection = "paper\n";
  } else { //scissors
        ledStateR = LOW;
        ledStateP = LOW;
        ledStateS = HIGH;
        computerSelection = "scissors\n";
  }
}

void lightshow() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis2 >= 250) {
    previousMillis2 = currentMillis;
    analogWrite(redPin, random(0,256));
    analogWrite(greenPin, random(0,256));
    analogWrite(bluePin , random(0,256));
  }
}
