#include <stdio.h>
#include <stdlib.h>
#include "sensor.h"

static const char filename[] = "ECG.txt";
static const FILE *file;

int getNextData(){
	int value;
	fscanf(file,"%i",&value);
	return value;
}

void openFile(){
		file = fopen(filename,"r");
}

void closeFile(){
	fclose(file);
}
