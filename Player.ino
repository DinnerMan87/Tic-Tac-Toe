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
    if (read == "2") {
      stage = "2";
      Serial.println("Stage 2");
      
      win = false;
      noTone(buzzer);

    } else if (read == "3") {
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

    /*if (win){
      tone(buzzer, 1000);

      analogWrite(RLED, random(255));
      analogWrite(GLED, random(255));
      analogWrite(BLED, random(255));
    }*/
  }

  if (newMill - oldMill > 500){
    oldMill = newMill;
    if (win){
      int a = analogRead(pMeter);
      Serial.println(a);
      if (a<1000){
        tone(buzzer, 0);
      } else{
        tone(buzzer , 500);
      }

      analogWrite(RLED, random(255));
      analogWrite(GLED, random(255));
      analogWrite(BLED, random(255));
    }
  }
  /*
  digitalWrite(rockLED, HIGH);
  digitalWrite(paperLED, HIGH);
  digitalWrite(scissorsLED, HIGH);

  analogWrite(RLED, 215);
  analogWrite(GLED, 105);
  analogWrite(BLED, 55);

  //tone(buzzer, 1000);

  Serial.print("\n\n\n\n\n\npMeter:");
  Serial.println(analogRead(pMeter));

  Serial.print("rockButton:");
  Serial.println(digitalRead(rockButton));

  Serial.print("paperButton:");
  Serial.println(digitalRead(paperButton));

  Serial.print("scissorsButton:");
  Serial.println(digitalRead(scissorsButton));

  delay(1000);
  */
}
