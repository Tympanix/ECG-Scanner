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
unsigned int THRESHOLD1 = 0;
unsigned int THRESHOLD2 = 0;

int RR_AVG1 = 0;
int RR_AVG2 = 0;
int RR_LOW = 0;
int RR_HIGH = 0;
int RR_MISS = -1;

int SLOPEUP = 0;
int RR;

buff rawIn;
buff lowPassOut;
buff highPassOut;
buff derivateOut;
buff squaringOut;
buff mwiOut;

buffPeak peaks;
buffPeak rpeaks;

buff recentRR;
buff recentRR_OK;

int main(int argc, char *argv[]){

	// Initialise buffer sizes
	initBuffer(13,&rawIn);
	initBuffer(33,&lowPassOut);
	initBuffer(5,&highPassOut);
	initBuffer(1,&derivateOut);
	initBuffer(30,&squaringOut);
	initBuffer(30,&mwiOut);

	// Initialise peaks buffer
	initBufferPeak(500, &peaks);
	initBufferPeak(12, &rpeaks);

	// Initialise RR buffers
	initBuffer(8, &recentRR);
	initBuffer(8, &recentRR_OK);

	openFile();
	openTestFiles();

	// Preprocess the first 2 datapoints
	filterNextData();
	filterNextData();

	unsigned long int time;
	for(time = 0; time < 2500; time++){
		filterNextData();

		int current = getHeadBuffer(&mwiOut);
		int last = getPreviousBuffer(1, &mwiOut);

		printf("%d\tValue: %d", time-1, last);

		if (current < last && SLOPEUP){
			// Maxima found
			Peak peak = {last, time-1};
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
	
	cleanupBuffer(&recentRR);
	cleanupBuffer(&recentRR_OK);

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
	return getAvgBuffer(&recentRR);
}

int calculateRRAVG2() {
	return getAvgBuffer(&recentRR_OK);
}

void updateNewRPeak(Peak peak) {
	insertToBufferPeak(peak, &rpeaks);
	insertToBuffer(RR, &recentRR);
	insertToBuffer(RR, &recentRR_OK);
	SPKF = peak.value/8 + 7*SPKF/8;
	RR_AVG1 = calculateRRAVG1();
	RR_AVG2 = calculateRRAVG2();
	updateLowHighMiss(RR_AVG2);
	updateThresholds();
}

Peak findPeakSearchback(int * result) {
	unsigned int i;
	for (i = 1; i < peaks.size; i++) {
		Peak peak = getPreviousPeak(i, &peaks);
		if (peak.value > THRESHOLD2) {
			printf(" to time=%d", peak.time);
			RR = calculateRR(peak);
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

	
	insertToBufferPeak(peak, &rpeaks);
	SPKF = peak.value/4 + 3*SPKF/4;
	insertToBuffer(RR, &recentRR);
	RR_AVG1 = calculateRRAVG1();
	printf(" RR_AVG1=%d", RR_AVG1);
	updateLowHighMiss(RR_AVG1);
	updateThresholds();

}

int calculateRR(Peak peak) {
	Peak last = getHeadPeak(&rpeaks);
	if (last.time == 0) return 0;
	RR = peak.time - last.time;
}

void foundRPeak(Peak peak) {
	RR = calculateRR(peak);

	printf(" (RR=%d, LOW=%d, HIGH=%d)\t", RR, RR_LOW, RR_HIGH);

	if (RR > RR_LOW && RR < RR_HIGH) {
		printf(" ***RPeak!");
		updateNewRPeak(peak);
	}
	else {
		if (RR > RR_MISS) {
			printf(" ***Searchback");
			searchBack();
			return;
		}
		printf(" - Under R_MISS, do nothing (RR=%d, MISS=%d)", RR, RR_MISS);	
	}
}

void updateNoRPeak(Peak peak) {
	NPKF = (peak.value/8) + (7*NPKF/8);
	updateThresholds();
}




