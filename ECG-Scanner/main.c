#include <stdio.h>
#include "sensor.h"
#include "buffer.h"
#include "filters.h"
#include "testData.h"

int main(int argc, char *argv[]){

	buff rawIn;
	initBuffer(13,&rawIn);

	buff lowPassOut;
	initBuffer(33,&lowPassOut);

	buff highPassOut;
	initBuffer(5,&highPassOut);

	buff derivateOut;
	initBuffer(1,&derivateOut);

	buff squaringOut;
	initBuffer(30,&squaringOut);

	buff movingWindowOut;
	initBuffer(30,&movingWindowOut);

	openFile();
	openTestFiles();
	int abc;
	for(abc = 0; abc < 40; abc++){
		int raw = getNextData();
		insertToBuffer(raw,&rawIn);
		lowPassFilter(&rawIn,&lowPassOut);
		highPassFilter(&lowPassOut,&highPassOut);
		derivativeFilter(&highPassOut,&derivateOut);
		squaringFilter(&derivateOut,&squaringOut);
		movingWindowFilter(&squaringOut,&movingWindowOut);
		printf("%d: \t raw: %d \t low: %d \t high: %d \t der: %d \t sqr: %d \t mwi: %d \n",abc,
				getHead(&rawIn),getHead(&lowPassOut), getHead(&highPassOut), getHead(&derivateOut),
				getHead(&squaringOut), getHead(&movingWindowOut));
		printf("%de: \t raw: %d \t low: %d \t high: %d \t der: %d \t sqr: %d \t mwi: %d \n",abc,
				raw, testLowData(),testHighData(), testDerivativeData(), testSquaringData(),
						testMovingWindowData());
	}





	closeFile();
	closeTestFile();





	cleanupBuffer(&rawIn);
	cleanupBuffer(&lowPassOut);
	cleanupBuffer(&highPassOut);
	cleanupBuffer(&derivateOut);
	cleanupBuffer(&squaringOut);
	cleanupBuffer(&movingWindowOut);
	return 0;

}
