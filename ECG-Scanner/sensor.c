#include <stdio.h>
#include <stdlib.h>
#include "sensor.h"

static const char filename[] = "ECG900K.txt";
static const FILE *file;

int getNextData(){
	int value;
	fscanf(file,"%i",&value);
	return value;
}

int openFile(char filename[]){
		file = fopen(filename,"r");
		if (file == NULL) {
			return 0;
		}
		return 1;
}

void closeFile(){
	fclose(file);
}

int endOfFile(){
	return feof(file);
}
