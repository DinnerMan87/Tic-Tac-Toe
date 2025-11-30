#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
//RefereeArduinoCode
#define rxPin 4
#define txPin 5

SoftwareSerial compSerial = SoftwareSerial(rxPin,txPin); //goes to computer port


const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

const int startButtonPin = 13;
int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int photoPin = A0;

String stage = "stage0"; //data sent from Referee of stage the game is on
String numOfRounds = "round0"; //data sent from Referee of rounds played so far
String prevMoves; //data sent from Referee of previous moves player made
                   //0=no moves played, r=rock. p=paper, s=scissors
String winner = "noWinner"; //data sent from Referee of who the winner is

String playerSelection = "?";
String computerSelection = "?";

char* cString;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //player port
  compSerial.begin(9600); //computer port
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  pinMode(startButtonPin, INPUT);
  lcd.begin(16,2);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (stage == "stage0") {

    int reading = digitalRead(startButtonPin);
    if (reading != lastButtonState){
      lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != buttonState) {
        buttonState = reading;
        if(buttonState == HIGH) {
          stage = "stage1";
          convert(stage);
          Serial.write(cString);
          mySerial.write(cString);
        }
      }
    }
    lastButtonState = reading;

  } else if (stage == "stage1") { //nothing for ref to do except wait for computer arduino

    if (compSerial.available() > 0) {
      if (compSerial.readStringUntil('\n') == "nextStage") {
        stage = "stage2";
      }
    }

  } else if (stage == "stage2") {
    
    if(Serial.available() > 0) {
      
    }
    if(compSerial.available() > 0){

    }

  } else if (stage == "stage3") {
    
  } else if (stage == "stage4") {
    
  }
}

void convert(String s) { //convert a C++ string to a cString to be sent Serially
  free(cString);
  cString = (char*) malloc(sizeof(char)*(s.length()+1));
  s.toCharArray(cString, s.length()+1);
}
