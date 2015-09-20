#include <stdio.h>
#include "sensor.h"
#include "buffer.h"
#include "filters.h"
#include "testData.h"
#include "peak.h"
#include "dynamicList.h"
#include "bufferPeak.h"
#include "main.h"

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
	initBufferPeak(12, &rpeaks_ok);

	openFile();
	openTestFiles();

	// Preprocess the first 2 datapoints
	filterNextData();
	filterNextData();

	int time;
	for(time = 0; time < 2000; time++){
		filterNextData();

		int current = getHeadBuffer(&mwiOut);
		int last = getPreviousBuffer(1, &mwiOut);

		printf("%d\tValue: %d", time, last);

		if (current < last && SLOPEUP){
			// Maxima found
			Peak peak = {last, time};
			insertToBufferPeak(peak, &peaks);
			//printf("New peak - value: %d\ttime: %d", peak.value, peak.time);
			printf(" *** PEAK!");
			if (peak.value > THRESHOLD1){
				// R peak found!
				printf(" - Possible RPeak");
				foundRPeak(peak);
			} else {
				// No R Peak found!
				updateNoRPeak(peak);
			}

			//printf("\n");
			SLOPEUP = 0;
		} else if (current > last) {
			SLOPEUP = 1;
		}

		printf("\n");
		/*
		printf("%d: \t raw: %d \t low: %d \t high: %d \t der: %d \t sqr: %d \t mwi: %d \n",abc,
				getHead(&rawIn),getHead(&lowPassOut), getHead(&highPassOut), getHead(&derivateOut),
				getHead(&squaringOut), getHead(&movingWindowOut));
		printf("%de: \t raw: %d \t low: %d \t high: %d \t der: %d \t sqr: %d \t mwi: %d \n",abc,
				raw, testLowData(),testHighData(), testDerivativeData(), testSquaringData(),
						testMovingWindowData());

		*/
	}

	/*
	int i;
	for(i = 0; i < peaks.size; i++){
		printf("peak value: %d \n", getPreviousPeak(i, &peaks).value);
	}
	*/


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

	getchar();
	return 0;

}


void filterNextData() {
	int raw = getNextData();
	insertToBuffer(raw, &rawIn);
	lowPassFilter(&rawIn, &lowPassOut);
	highPassFilter(&lowPassOut, &highPassOut);
	derivativeFilter(&highPassOut, &derivateOut);
	squaringFilter(&derivateOut, &squaringOut);
	movingWindowFilter(&squaringOut, &mwiOut);
}

void updateThresholds() {
	THRESHOLD1 = NPKF + (SPKF - NPKF) / 4;
	THRESHOLD2 = THRESHOLD1 / 2;
}

void updateLowHighMiss(int AVG) {
	RR_LOW = 92 * AVG / 100;
	RR_HIGH = 116 * AVG / 100;
	RR_MISS = 166 * AVG / 100;
}

int calculateRRAVG1() {
	return getAverageRRPeak(&rpeaks, 8);
}

int calculateRRAVG2() {
	return getAverageRRPeak(&rpeaks_ok, 8);
}

void updateNewRPeak(Peak peak) {
	insertToBufferPeak(peak, &rpeaks);
	insertToBufferPeak(peak, &rpeaks_ok);
	SPKF = peak.value/8 + 7*SPKF/8;
	RR_AVG1 = calculateRRAVG1();
	RR_AVG2 = calculateRRAVG2();
	updateLowHighMiss(RR_AVG2);
	updateThresholds();
}

Peak findPeakSearchback(int * result) {
	int i;
	for (i = 1; i < peaks.size; i++) {
		Peak peak = getPreviousPeak(i, &peaks);
		if (peak.value > THRESHOLD2) {
			printf(" to time=%d", peak.time);
			insertToBufferPeak(peak, &rpeaks);
			*result = 1;
			return peak;
		}
	}

	*result = 0;
	Peak peak = { 0,0 };
	return peak;
}

void searchBack() {
	int result;
	Peak peak = findPeakSearchback(&result);

	if (!result) {
		printf(" nothing found!");
		return;
	}
	
	SPKF = peak.value/4 + 3*SPKF/4;
	RR_AVG1 = calculateRRAVG1();
	printf(" RR_AVG1=%d", RR_AVG1);
	updateLowHighMiss(RR_AVG1);
	updateThresholds();

}

void foundRPeak(Peak peak) {
	int rr = peak.time - getHeadPeak(&rpeaks).time;

	if (rr > RR_LOW && rr < RR_HIGH) {
		printf(" ***RPeak!");
		updateNewRPeak(peak);
	}
	else {
		printf(" - Outside RR (RR=%d, LOW=%d, HIGH=%d)\t", rr, RR_LOW, RR_HIGH);
		if (rr > RR_MISS) {
			printf(" ***Searchback");
			searchBack();
			return;
		}
		printf(" - Under R_MIIS, do nothing");
		
	}
}

void updateNoRPeak(Peak peak) {
	NPKF = (peak.value/8) + (7*NPKF/8);
	updateThresholds();
}




