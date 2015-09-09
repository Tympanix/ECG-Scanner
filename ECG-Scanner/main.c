#include <stdio.h>
#include "sensor.h"
#include "buffer.h"

int main(int argc, char *argv[]){
	/* Print hello world to the screen */
	printf("Hello Gabriel & Marcus\n");

	openFile();

	printf("Data er: %d\n", getNextData());
	printf("Data er: %d\n", getNextData());
	printf("Data er: %d\n", getNextData());

	closeFile();

	buff testB;

	initBuffer(10, &testB);

	insertToBuffer(1,&testB);
	insertToBuffer(5,&testB);
	insertToBuffer(3,&testB);
	insertToBuffer(4,&testB);

	printf("%d \n" , getPreviousByIntFromBuffer(2,&testB));
	printf("%d \n" , getPreviousByIntFromBuffer(6,&testB));
	cleanupBuffer(&testB);

	return 0;

}
