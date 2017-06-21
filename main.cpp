#import "mbed.h"
#include <stdio.h>
//#include "DebounceIn.h"

Serial pc(USBTX, USBRX);


DigitalIn button(D7);

DigitalIn group1(D5);
DigitalIn group2(D4);
DigitalIn group3(D3);
DigitalIn group4(D2);

int debounce = 20;
int holdTime = 1000;

int buttonVal = 1; // value read from button
int buttonLast = 1; // buffered value of the button's previous state
long btnDnTime; // time the button was pressed down
long btnUpTime; // time the button was released
bool ignoreUp = false; // whether to ignore the button release because the click+hold was triggered

char g1[] = {' ','A','B','C','D','E','F'};
char g2[] = {'G','H','I','J','K','L','M'};
char g3[] = {'N','O','P','Q','R','S','T'};
char g4[] = {'U','V','W','X','Y','Z',' '};

char userText[30]={0};
int alphabetCursor = -1;
int userCursor = 0;

//char chosengroup[];
char *chosengroup;
char *lastchosen;

Timer t;

void event_next(){
	if (chosengroup!=NULL){
		if (alphabetCursor == -1) {
			pc.printf("\r%s\n", " " );
		}

		if (alphabetCursor == 6){
			pc.printf("\033[1A");
			alphabetCursor = 0;
			pc.printf("\r%c\n", chosengroup[alphabetCursor] );
		} else {
			pc.printf("\033[1A");
			alphabetCursor++;
			pc.printf("\r%c\n", chosengroup[alphabetCursor] );
		}
	}
}

void event_enter(){
	userText[userCursor] = chosengroup[alphabetCursor];
	userCursor ++;
	alphabetCursor = -1;
	pc.printf("%s\n\r", userText);
	pc.printf("\033[2A");
}

int main(){
	t.start();
	pc.printf("Turn, Click, Hold to Type: \n\r");
	pc.printf("* ->   A B C D E F\n\r");
	pc.printf("2 -> G H I J K L M\n\r");
	pc.printf("3 -> N O P Q R S T\n\r");
	pc.printf("4 -> U V W X Y Z  \n\r");

	while (1){

		// check which group is chosen
		if (group1.read() == 0) {
			chosengroup = g1;
			if (lastchosen != chosengroup) {
				alphabetCursor = -1;
				lastchosen = chosengroup;
			}
		} else if (group2.read() == 0) {
			chosengroup = g2;
			if (lastchosen != chosengroup) {
				alphabetCursor = -1;
				lastchosen = chosengroup;
			}
		} else if (group3.read() == 0) {
			chosengroup = g3;
			if (lastchosen != chosengroup) {
				alphabetCursor = -1;
				lastchosen = chosengroup;
			}
		} else if (group4.read() == 0) {
			chosengroup = g4;
			if (lastchosen != chosengroup) {
				alphabetCursor = -1;
				lastchosen = chosengroup;
			}
		}


		// Read the state of the enter/next button
		buttonVal = button.read();
		// Test for button pressed and store the down time
		if (buttonVal == 0 && buttonLast == 1 && (t.read_ms()  - btnUpTime) > long(debounce)){
			btnDnTime = t.read_ms() ;
		}
		// Test for button release and store the up time
		if (buttonVal == 1 && buttonLast == 0 && (t.read_ms()  - btnDnTime) > long(debounce))
		{	
			// if click+hold is not triggered, function as 'next'
			if (ignoreUp == false) {
				event_next();
			} else {
				ignoreUp = false;
				btnUpTime = t.read_ms() ;
			}
		}

		// Test for button held down for longer than the hold time
		if (buttonVal == 0 && (t.read_ms()  - btnDnTime) > long(holdTime)){
			// if clicked and held, function as 'enter'
			event_enter();
			ignoreUp = true;
			btnDnTime = t.read_ms() ;
		}
		buttonLast = buttonVal;
	}
}