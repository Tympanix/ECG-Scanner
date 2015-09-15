#include <stdio.h>
#include <stdlib.h>
#include "testData.h"

//static const char filename[] = "testData/X.txt";
static const FILE *file_xder;
static const FILE *file_xhigh;
static const FILE *file_xlow;
static const FILE *file_xmwi;
static const FILE *file_xsqr;

int testLowData(){
	int value;
	fscanf(file_xlow,"%i",&value);
	return value;
}
int testHighData(){
	int value;
	fscanf(file_xhigh,"%i",&value);
	return value;
}
int testDerivativeData(){
	int value;
	fscanf(file_xder,"%i",&value);
	return value;
}
int testSquaringData(){
	int value;
	fscanf(file_xsqr,"%i",&value);
	return value;
}
int testMovingWindowData(){
	int value;
	fscanf(file_xmwi,"%i",&value);
	return value;
}

void openTestFiles(){
		file_xder = fopen("testData/x_der.txt","r");
		file_xlow = fopen("testData/x_low.txt","r");
		file_xhigh = fopen("testData/x_high.txt","r");
		file_xmwi = fopen("testData/x_mwi_div_after.txt","r");
		file_xsqr = fopen("testData/x_sqr.txt","r");
}

void closeTestFile(){
	fclose(file_xder);
	fclose(file_xlow);
	fclose(file_xhigh);
	fclose(file_xmwi);
	fclose(file_xsqr);
}
