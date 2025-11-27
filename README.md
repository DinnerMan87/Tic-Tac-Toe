Stage 0: Player selects if he wants to go against another player or against the computer
Stage 1: Branches between two players or computer and game starts
Stage 2: Player/s select what they want to play with a time limit of 10 seconds if not a selection is randomly selected. The computer will randomly select or if there is enough past player choices then
will base its choice on past selections
Stage 3: The referee announces the winner and a buzzer makes a sound
(Repeat stages 2 and 3 for 5 games)
Stage 4: Whoever won the best of 5 will be announced on the lcd screen and an rgb led will put on a show and the buzzer will go off

Use the SoftwareSerial library to connect all our arduinos together. Example of how to use it: 
#include <SoftwareSerial.h>

// Define RX and TX pins
const byte rxPin = 2;
const byte txPin = 3;

// Create a SoftwareSerial object
SoftwareSerial mySerial(rxPin, txPin);

void setup() {
  // Initialize the software serial port at 9600 baud
  mySerial.begin(9600);
  mySerial.println("SoftwareSerial Initialized");
}

void loop() {
  // Check if data is available to read
  if (mySerial.available() > 0) {
    char received = mySerial.read(); // Read incoming data
    mySerial.print("Received: ");
    mySerial.println(received);
  }
}
