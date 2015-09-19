#include <stdio.h>
#include "sensor.h"
#include "buffer.h"
#include "filters.h"
#include "testData.h"
#include "peak.h"
#include "dynamicList.h"
#include "bufferPeak.h"

int SPKF = 0;
int NPKF = 0;
int THRESHOLD1 = 0;
int THRESHOLD2 = 0;

int RR_AVG1 = 0;
int RR_AVG2 = 0;
int RR_LOW = 0;
int RR_HIGH = 0;
int RR_MISS = 0;

int SLOPEUP = 0;

buff rawIn;
buff lowPassOut;
buff highPassOut;
buff derivateOut;
buff squaringOut;
buff mwiOut;

buffPeak peaks;
buffPeak rpeaks;
buffPeak rpeaks_ok;


void filterNextData(){
	int raw = getNextData();
	insertToBuffer(raw,&rawIn);
	lowPassFilter(&rawIn,&lowPassOut);
	highPassFilter(&lowPassOut,&highPassOut);
	derivativeFilter(&highPassOut,&derivateOut);
	squaringFilter(&derivateOut,&squaringOut);
	movingWindowFilter(&squaringOut,&mwiOut);
}

void updateThresholds(){
	THRESHOLD1 = NPKF + (SPKF - NPKF)/4;
	THRESHOLD2 = THRESHOLD1/2;
	printf("Updated thresholds to %d and %d \n", THRESHOLD1, THRESHOLD2);
}

void updateLowHighMiss(int AVG){
	RR_LOW = 92*AVG/100;
	RR_HIGH = 116*AVG/100;
	RR_MISS = 166*AVG/100;
	printf("Updated Low %d, High %d, and Miss %d \n", RR_LOW, RR_HIGH, RR_MISS);
}

int calculateRRAVG1(){
	int count = 8;
	int sum = 0;
	int i;
	for (i = 0; i < count; i++){
		Peak peak1 = getPreviousPeak(i, &rpeaks);
		int previous = peak1.value;
		//int before = getPreviousPeak(i+1, &rpeaks).value;
		//sum += (first - before);
		sum += previous;
	}

	return sum/count;
}

int calculateRRAVG2(){
	int count = 8;
	int sum = 0;
	int i;
	for (i = 0; i < count; i++){
		int previous = getPreviousPeak(i, &rpeaks_ok).value;
		//int before = getPreviousPeak(i+1, &rpeaks_ok).value;
		//sum += (first - before);
		sum += previous;
	}

	return sum/count;
}

void updateNewRPeak(Peak peak){
	insertToBufferPeak(peak, &rpeaks);
	printf("Inserted peak %d into rpeaks, ", peak.value);
	insertToBufferPeak(peak, &rpeaks_ok);
	SPKF = peak.value/8 + 7*SPKF/8;
	printf("SPKF = %d \n", SPKF);
	RR_AVG1 = calculateRRAVG1();
	printf("new AVG1 is %d, ", RR_AVG1);
	RR_AVG2 = calculateRRAVG2();
	printf("new AVG2 is %d \n", RR_AVG2);
	updateLowHighMiss(RR_AVG2);
	updateThresholds();
}

int findPeakSearchback(){
	int i;
	for (i = 0; i < peaks.size; i++){
		Peak peak = getPreviousPeak(i,&peaks);
		if (peak.value > THRESHOLD2){
			insertToBufferPeak(peak, &rpeaks);
			printf("Inserted peak %d into rpeaks \n", peak.value);
			return peak.value;
		}
	}
	return 0;
}

void searchBack(){
	int value = findPeakSearchback();

	if(value != 0){
		SPKF = value/4 + 3*SPKF/4;
		printf("SPKF = %d \n", SPKF);
	}
	RR_AVG1 = calculateRRAVG1();
	updateLowHighMiss(RR_AVG1);
	updateThresholds();
}

void foundRPeak(Peak peak){
	int rr = peak.time - getHeadPeak(&rpeaks).time;

	if (rr > RR_LOW && rr < RR_HIGH){
		updateNewRPeak(peak);
		printf("Updated rpeaks \n");
	} else {
		if (rr > RR_MISS){
			searchBack();
			printf("Performed searchback \n");
		}
	}
}

void updateNoRPeak(Peak peak){
	printf("Peak value = %d, ", peak.value);
	NPKF = (0.125*peak.value) + (0.875*NPKF);
	printf("NPKF = %d \n", NPKF);
	updateThresholds();
}

int main(int argc, char *argv[]){


	// Initialise buffer sizes
	initBuffer(13,&rawIn);
	initBuffer(33,&lowPassOut);
	initBuffer(5,&highPassOut);
	initBuffer(1,&derivateOut);
	initBuffer(30,&squaringOut);
	initBuffer(30,&mwiOut);

	// Initialise peaks buffer;
	initBufferPeak(500, &peaks);
	initBufferPeak(12, &rpeaks);

	openFile();
	openTestFiles();

	// Preprocess the first 2 datapoints
	//filterNextData();
	//filterNextData();

	int time;
	for(time = 0; time < 1000; time++){

		filterNextData();

		int current = getHeadBuffer(&mwiOut);
		int last = getPreviousBuffer(1, &mwiOut);

		if (current < last && SLOPEUP){
			// Maxima found
			Peak peak = {last, time};
			insertToBufferPeak(peak, &peaks);

			if (peak.value > THRESHOLD1){
				printf("Found new rpeak \n");
				foundRPeak(peak);
			} else {
				// No R Peak found
				printf("Found no rpeak \n");
				updateNoRPeak(peak);
			}

			SLOPEUP = 0;
		} else if (current > last) {
			SLOPEUP = 1;
		}




		/*
		printf("%d: \t raw: %d \t low: %d \t high: %d \t der: %d \t sqr: %d \t mwi: %d \n",abc,
				getHead(&rawIn),getHead(&lowPassOut), getHead(&highPassOut), getHead(&derivateOut),
				getHead(&squaringOut), getHead(&movingWindowOut));
		printf("%de: \t raw: %d \t low: %d \t high: %d \t der: %d \t sqr: %d \t mwi: %d \n",abc,
				raw, testLowData(),testHighData(), testDerivativeData(), testSquaringData(),
						testMovingWindowData());

		*/
	}
	int i;
	for(i = 0; i < rpeaks.size; i++){
		printf("peak value: %d \n", getPreviousPeak(i, &rpeaks).value);
	}


	// Close files and cleanup!
	closeFile();
	closeTestFile();

	cleanupBuffer(&rawIn);
	cleanupBuffer(&lowPassOut);
	cleanupBuffer(&highPassOut);
	cleanupBuffer(&derivateOut);
	cleanupBuffer(&squaringOut);
	cleanupBuffer(&mwiOut);

	cleanupBufferPeak(&peaks);
	cleanupBufferPeak(&rpeaks);
	cleanupBufferPeak(&rpeaks_ok);

	return 0;

}




