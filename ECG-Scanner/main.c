#include <stdio.h>
#include "sensor.h"
#include "buffer.h"
#include "filters.h"
#include "testData.h"
#include "peak.h"
#include "dynamicList.h"
#include "bufferPeak.h"

int SPKF;
int NPKF;
int THRESHOLD1;
int THRESHOLD2;

int RR_AVG1;
int RR_AVG2;
int RR_LOW;
int RR_HIGH;
int RR_MISS;

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
	initBufferPeak(250, &peaks);
	initBufferPeak(12, &rpeaks);

	openFile();
	openTestFiles();

	// Preprocess the first 2 datapoints
	filterNextData();
	filterNextData();

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
				foundRPeak(peak);
			} else {
				// No R Peak found
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
		printf("peak value: %d \n", getPreviousBuffer(i,&rpeaks));
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

void filterNextData(){
	int raw = getNextData();
	insertToBuffer(raw,&rawIn);
	lowPassFilter(&rawIn,&lowPassOut);
	highPassFilter(&lowPassOut,&highPassOut);
	derivativeFilter(&highPassOut,&derivateOut);
	squaringFilter(&derivateOut,&squaringOut);
	movingWindowFilter(&squaringOut,&mwiOut);
}

void foundRPeak(Peak peak){
	int rr = peak.time - getHeadPeak(&rpeaks).time;

	if (rr > RR_LOW && rr < RR_HIGH){
		updateNewRPeak(peak);
	} else {
		if (rr > RR_MISS){
			searchBack();
		}
	}
}

void updateNoRPeak(Peak peak){
	NPKF = (peak.value)/8 + (7*NPKF/8);
	updateThresholds();
}

void updateThresholds(){
	THRESHOLD1 = NPKF + (SPKF - NPKF)/4;
	THRESHOLD2 = THRESHOLD2/2;
}

void updateNewRPeak(Peak peak){
	insertToBufferPeak(peak, &rpeaks);
	insertToBufferPeak(peak, &rpeaks_ok);
	SPKF = peak.value/8 + 7*SPKF/8;
	RR_AVG1 = calculateRRAVG1();
	RR_AVG2 = calculateRRAVG2();
	updateLowHighMiss();
	updateThresholds();
}

void updateLowHighMiss(){
	RR_LOW = 92*RR_AVG2/100;
	RR_HIGH = 116*RR_AVG2/100;
	RR_MISS = 166*RR_AVG2/100;
}

int calculateRRAVG1(){
	int count = 8;
	int sum = 0;
	int i;
	for (i = 0; i < count; i++){
		Peak peak1 = getPreviousPeak(i, &rpeaks);
		int first = peak1.value;
		int before = getPreviousPeak(i+1, &rpeaks).value;
		sum += (first - before);
	}

	return sum/count;
}

int calculateRRAVG2(){
	int count = 8;
	int sum = 0;
	int i;
	for (i = 0; i < count; i++){
		int first = getPreviousPeak(i, &rpeaks_ok).value;
		int before = getPreviousPeak(i+1, &rpeaks_ok).value;
		sum += (first - before);
	}

	return sum/count;
}

void searchBack(){
	int value = findPeakSearchback();

	if(value != 0){
		SPKF = value/4 + 3*SPKF/4;
	}
	RR_AVG1 = calculateRRAVG1();
	updateLowHighMiss();
	updateThresholds();
}

int findPeakSearchback(){
	int i;
	for (i = 0; i < peaks.size; i++){
		Peak peak = getPreviousPeak(i,&peaks);
		if (peak.value > THRESHOLD2){
			insertToBufferPeak(peak, &rpeaks);
			return peak.value;
		}
	}
	return 0;
}





