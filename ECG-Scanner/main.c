#include <stdio.h>
#include "sensor.h"

int main(int argc, char *argv[]){
	/* Print hello world to the screen */
	printf("Hello Gabriel & Marcus\n");

	openFile();

	printf("Data er: %d\n", getNextData());
	printf("Data er: %d\n", getNextData());
	printf("Data er: %d\n", getNextData());

	closeFile();



	return 0;

}
