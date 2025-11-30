//LEDS
const int rockLED = 5, paperLED = 6, scissorsLED = 7;
const int RLED = 9, GLED = 10, BLED = 11;

//Buttons
const int rockButton = 2, paperButton = 3, scissorsButton = 4;

//Other Devices
const int pMeter = A0, buzzer = 8;

//MISC
String choice = "none\n";
const String stage = "?";
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
}

int oldMill = 0;
void loop() {
  int newMill = millis();

  if (Serial.available() > 0) {
    String read = Serial.readStringUntil('\n');
    if (read == "stage2") {
      stage = "2";
      Serial.println("Stage 2");
      
      win = false;
      noTone(buzzer);

    } else if (read == "stage3") {
      stage = "3";
      Serial.print("Stage 3: sending player choice - ");
      Serial.write(choice.c_str());
    } else if (read == "win"){
      win = true;
      Serial.println("WON");
    }
  }

  if (stage == "2"){
    
    if (digitalRead(rockButton) == 1){
      choice = "rock\n";
      digitalWrite(rockLED, HIGH);

      digitalWrite(paperLED, LOW);
      digitalWrite(scissorsLED, LOW);
    }else if (digitalRead(paperButton) == 1){
      choice = "paper\n";
      digitalWrite(paperLED, HIGH);

      digitalWrite(rockLED, LOW);
      digitalWrite(scissorsLED, LOW);
    }else if (digitalRead(scissorsButton) == 1){
      choice = "scissors\n";
      digitalWrite(scissorsLED, HIGH);

      digitalWrite(rockLED, LOW);
      digitalWrite(paperLED, LOW);
    } 


  } else if (stage == "3"){
    digitalWrite(rockLED, LOW);
    digitalWrite(paperLED, LOW);
    digitalWrite(scissorsLED, LOW);
  }

  if (newMill - oldMill > 100){
    oldMill = newMill;
    if (win){
      winCycle++;

      analogWrite(RLED, random(255));
      analogWrite(GLED, random(255));
      analogWrite(BLED, random(255));

      int a = analogRead(pMeter);
      //Serial.println(a);
      if (a > 50)
        tone(buzzer, a);
      else
        noTone(buzzer);

      if (winCycle >= 20){
        win = false;
        noTone(buzzer);
        winCycle = 0;
        analogWrite(RLED, 0);
        analogWrite(GLED, 0);
        analogWrite(BLED, 0);
      }
    }
  }
}

