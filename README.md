
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

Stage 0: Player selects if he wants to go against another player or against the computer
Stage 1: Branches between two players or computer and game starts
Stage 2: Player/s select what they want to play with a time limit of 10 seconds if not a selection is randomly selected. The computer will randomly select or if there is enough past player choices then
will base its choice on past selections
Stage 3: The referee announces the winner and a buzzer makes a sound
(Repeat stages 2 and 3 for 5 games)
Stage 4: Whoever won the best of 5 will be announced on the lcd screen and an rgb led will put on a show and the buzzer will go off

Note: 0=nothings played, r=rock, p=paper, s=scissors
