/*Program: Simon 
 *Purpose: Plays a game of Simon after initializing DAQ
 */

#define _CRT_SECURE_NO_WARNINGS

#define TRUE 1
#define FALSE 0

#define GREEN_LED0 0
#define RED_LED1 1
#define YELLOW_LED2 2
#define BLUE_LED3 3

#define PUSH_BUTTON0 0
#define PUSH_BUTTON1 1
#define PUSH_BUTTON2 2
#define PUSH_BUTTON3 3

#define SEQUENCE_LENGTH 5
#define WIN_LOSE_SEQUENCE_LENGTH 3

#define SWITCH_ON 1
#define SWITCH_OFF 0

#define GAME_START_INTERVAL 2000
#define WIN_LOSE_INTERVAL 300
#define SEQUENCE_INTERVAL 600

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h> 
#include <time.h>
#include <DAQlib.h>

int randInt(int lower, int upper);
int checkSequence(int chosenSequence [], int sequence [], int sequenceIndex);
int inputButton(int chosenSequence[], int chosenIndex, int button);
void runSimon(void);
void generateSequence(int length, int data[]);
void flashSequence(int sequence [], int sequenceIndex, int interval);
void inputSequence(int chosenSequence [], int sequenceIndex);


int main(void){
	int setupNum;

	printf("Enter configuration type (0 for the device or 6 for the simulator): ");
	scanf("%d", &setupNum);

	if (setupDAQ(setupNum) == TRUE)
		runSimon();
	else
		printf("ERROR: Cannot initialize system\n");

	system("PAUSE");
	return 0;
	
}

// Runs an instance of Simon
void runSimon(void){
	int lose[] = { RED_LED1, RED_LED1, RED_LED1 };
	int win[] = { GREEN_LED0, GREEN_LED0, GREEN_LED0};
	int sequence[SEQUENCE_LENGTH];
	int chosenSequence[SEQUENCE_LENGTH];

	srand(time(NULL)); //Seeds PRNG once before Simon is played.

	while (continueSuperLoop() == TRUE){
		int continueSequence = TRUE;
		int sequenceIndex = 0;  //Starts at round 0

		generateSequence(SEQUENCE_LENGTH, sequence);  //Add randomized values to sequence
		Sleep(GAME_START_INTERVAL);  //Pauses the game for a short duration allowing the player to get ready 

		while (continueSequence == TRUE) {   //Starts a LED sequence loop, ending when whether the user gets all the sequences right or loses before then
			int chosenIndex = 0;   //Reset chosen index to 0
			int correctSequence;

			flashSequence(sequence, sequenceIndex, SEQUENCE_INTERVAL); //Flashes next sequence of LEDs
			inputSequence(chosenSequence, sequenceIndex); //Records button presses
			correctSequence = checkSequence(chosenSequence, sequence, sequenceIndex); //Checks if both sequences are the same
				
			if (correctSequence == TRUE){
				if (sequenceIndex == SEQUENCE_LENGTH - 1){
					flashSequence(win, WIN_LOSE_SEQUENCE_LENGTH, WIN_LOSE_INTERVAL);
					continueSequence = FALSE;
				}
				else
					sequenceIndex++;
				}
			else{
				flashSequence(lose, WIN_LOSE_SEQUENCE_LENGTH, WIN_LOSE_INTERVAL);
				continueSequence = FALSE;
			}

		}
	
	}
}

// Returns a random number between a lower boundary (inclusive), lower, and an upper boundary (incusive), upper.
int randInt(int lower, int upper){
	return (rand() % (upper - lower + 1) + lower);
}


// Returns TRUE if the array, chosenSequence, is equal to the flashed sequence, sequence, up to index, sequenceIndex and FALSE otherwise. 
int checkSequence(int chosenSequence[], int sequence[], int sequenceIndex) {
	for (int i = 0; i <= sequenceIndex; i++){
		if (chosenSequence[i] != sequence[i])
			return FALSE;
	}
	return TRUE;
}

// Generates an array, data, of length, length, with elements containing randomized LEDs
void generateSequence(int length, int data[]){
	for (int index = 0; index < length; index++)
		data[index] = randInt(GREEN_LED0, BLUE_LED3);
}

// Flashes an array, sequence, of LEDs up to a specified index, sequenceIndex, for a duration of interval.
void flashSequence(int sequence[], int sequenceIndex, int interval){
	for (int i = 0; i <= sequenceIndex; i++){
		digitalWrite(sequence[i], SWITCH_ON);
		Sleep(interval);
		digitalWrite(sequence[i], SWITCH_OFF);

		if (i != sequenceIndex)	//Don't sleep for last LED (creates lag otherwise)
			Sleep(interval);
	}
}


// Records the user's button sequences into an array, chosenSequence, of length, sequenceIndex
void inputSequence(int chosenSequence[], int sequenceIndex){
	int chosenIndex = 0;

	while (chosenIndex <= sequenceIndex){    //loops through amount of button choices
		if (continueSuperLoop() == FALSE) //Ensures the function stops if DAQ is closed
			return;

		for (int button = PUSH_BUTTON0; button <= PUSH_BUTTON3; button++){
			if (inputButton(chosenSequence, chosenIndex, button) == TRUE)
				chosenIndex++;
		}
	}
}

// Returns TRUE if a button is pressed and places an element, button, into the index, chosenIndex, of array, chosenSequence. 
int inputButton(int chosenSequence[], int chosenIndex, int button){ 
	if (digitalRead(button) == SWITCH_ON){ //Check if a button is pressed
		while (digitalRead(button) == SWITCH_ON){}		//Wait until button is released before moving on

		chosenSequence[chosenIndex] = button; //Assign button press
		return TRUE;
	}
}


