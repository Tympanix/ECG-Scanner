#include <stdio.h>
#include "sensor.h"
#include "buffer.h"
#include "buffer64bit.h"
#include "filters.h"
#include "testData.h"
#include "peak.h"
#include "dynamicList.h"
#include "bufferPeak.h"
#include "main.h"

#define RELEASE
//#define DEBUG
//#define DEBUG_FILTERS
#define DEBUG_START 10594000
#define DEBUG_STOP 10598000


unsigned long int time = 0;

int SPKF = 2000;
int NPKF = 250;
unsigned int THRESHOLD1 = 1000;
unsigned int THRESHOLD2 = 500;

int RR_AVG1 = 0;
int RR_AVG2 = 0;
int RR_LOW = 0;
int RR_HIGH = 0;
int RR_MISS = -1;

int SLOPEUP = 0;
int RR;
int NUM_MISS = 0;
int TIME_MISS_IGNORE = 0;
int TIME_LAST_RPEAK = 0;
int HEARTATTACK = 0;
const int INITIAL_RR_INTERVAL = 50;

buff rawIn;
buff64bit lowPassOut;
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
	initBuffer64bit(33,&lowPassOut);
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

	while(!endOfFile()){
		filterNextData();

		int current = getHeadBuffer(&mwiOut);
		int last = getPreviousBuffer(1, &mwiOut);

		#ifdef DEBUG
		if (debug(time)) printf("%d\tValue: %d", time-1, last);
		#endif

		if (current < last && SLOPEUP){
			// Maxima found
			SLOPEUP = 0;
			Peak peak = {last, time-1};
			insertToBufferPeak(peak, &peaks);
			checkHeartAttack();

			#ifdef DEBUG
			if (debug()) printf(" *** PEAK!");
			#endif

			if (ignorePeakSpike(peak)) goto skip_peak;

			if (peak.value > THRESHOLD1){
				// Possible RPeak 
				#ifdef DEBUG
				if (debug()) printf(" - Possible RPeak");
				#endif
				possibleRPeak(peak);
			} else {
				// Not a RPeak, update thresholds
				updateNoRPeak(peak);
			}

		} else if (current > last) {
			SLOPEUP = 1;
		}
		
		skip_peak:
		time++;

		#ifndef RELEASE
		if (time >= DEBUG_STOP) {
			break;
		}
		#endif
		
		# ifdef DEBUG
		if (debug()) printf("\n");
		#endif

	}


	// Close files and cleanup!
	closeFile();
	closeTestFile();

	cleanupBuffer(&rawIn);
	cleanupBuffer(&lowPassOut);
	cleanupBuffer64bit(&highPassOut);
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

void checkHeartAttack() {
	if (time - TIME_LAST_RPEAK > RR_HIGH * 5 && !HEARTATTACK) {
		HEARTATTACK = 1;
		#ifdef RELEASE
		printf("Warning! Heart stop\n");
		#endif
	}
}

int ignorePeakSpike(Peak peak) {
	if (peak.value > SPKF * 10) {
		#ifdef DEBUG
		if (debug()) printf(" - Peak too large SPKF=%d, NPKF=%d, THR1=%d, THR2=%d", SPKF, NPKF, THRESHOLD1, THRESHOLD2);
		#endif
		return 1;
	}
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

	#ifdef DEBUG_FILTERS
	if (debug() && time >= DEBUG_START) {
		printf("Time: %d\t\t raw=%d\t low=%lld\t high=%d\t der=%d\t sqr=%d\t mwi=%d\n", time, getHeadBuffer(&rawIn), getHeadBuffer64bit(&lowPassOut),
			getHeadBuffer(&highPassOut), getHeadBuffer(&derivateOut), getHeadBuffer(&squaringOut), getHeadBuffer(&mwiOut));

	}
	#endif


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
	
	#ifdef RELEASE
	updateGUI(peak);
	#endif
}

Peak findPeakSearchback(int * result) {
	unsigned int i;
	for (i = 1; i < peaks.size; i++) {
		Peak peak = getPreviousPeak(i, &peaks);
		if (peak.value > THRESHOLD2) {
			*result = 1;
			return peak;
		}
	}

	*result = 0;
	Peak peak = { 0,0 };
	return peak;
}

void searchBack(Peak peak) {
	int result;
	Peak peak2 = findPeakSearchback(&result);

	if (!result) {
		// No peak2 found in searchback (the very first RPeak will end here)!
		#ifdef DEBUG
		if (debug()) printf(" nothing found!");
		#endif

		// Help algorith towards next RPeak!
		RR_MISS = INITIAL_RR_INTERVAL;
		insertToBufferPeak(peak, &rpeaks);
		return;
	}

	#ifdef DEBUG
	if (debug()) printf(" to time=%d", peak2.time);
	#endif

	RR = peak.time - peak2.time;

	insertToBufferPeak(peak, &rpeaks);
	SPKF = peak.value/4 + 3*SPKF/4;
	insertToBuffer(RR, &recentRR);
	RR_AVG1 = calculateRRAVG1();
	updateLowHighMiss(RR_AVG1);
	updateThresholds();

	#ifdef RELEASE
	updateGUI(peak);
	#endif

	HEARTATTACK = 0;
}

int calculateRR(Peak peak) {
	Peak last = getHeadPeak(&rpeaks);
	if (last.time == 0) return 0;
	return peak.time - last.time;
}

void possibleRPeak(Peak peak) {
	TIME_LAST_RPEAK = time;
	RR = calculateRR(peak);
	
	#ifdef DEBUG
	if (debug()) printf("(RR=%d, LOW=%d, HIGH=%d)\t", RR, RR_LOW, RR_HIGH);
	#endif

	if (RR > RR_LOW && RR < RR_HIGH) {
		// Normal RPeak found!
		#ifdef DEBUG
		if (debug()) printf(" ***RPeak!");
		#endif
		updateNewRPeak(peak);
		NUM_MISS = 0;
		HEARTATTACK = 0;
		TIME_MISS_IGNORE = time;
	} else {
		checkMissedHeartbeat();
		if (RR > RR_MISS) {
			// Missed, do searchback
			#ifdef DEBUG
			if (debug()) printf(" ***Searchback");
			#endif
			searchBack(peak);
			return;
		}

	#ifdef DEBUG
	if (debug()) printf(" - Under R_MISS, do nothing (RR=%d, MISS=%d)", RR, RR_MISS);
	#endif

	}

}

void checkMissedHeartbeat() {
	if (time - TIME_MISS_IGNORE > 25) {
		NUM_MISS++;
		TIME_MISS_IGNORE = time;
		#ifdef DEBUG
		if (debug()) printf(" - Increment NUM_MISS");
		#endif
	}
}

void updateNoRPeak(Peak peak) {
	NPKF = (peak.value/8) + (7*NPKF/8);
	updateThresholds();

	#ifdef DEBUG
	if (debug()) printf(" - SPKF=%d, NPKF=%d, THR1=%d, THR2=%d", SPKF, NPKF, THRESHOLD1, THRESHOLD2);
	#endif
}

void updateGUI(Peak peak){
	printf("Rpeak\ttime = %d\t value = %d\t pulse = %d ", peak.time, peak.value, updatePulse());
	if(peak.value < 2000) {
		printf("\tWarning! Rpeak value is too low ");
	}
	if(NUM_MISS >= 5) {
		printf("\tWarning! Rpeak has missed 5 or more intervals ");
	}
	printf("\n");
}

int updatePulse() {
	if(RR_AVG1 == 0) return 0;
	return 250*60 / RR_AVG1;
}


int debug() {
	return time >= DEBUG_START;
}





