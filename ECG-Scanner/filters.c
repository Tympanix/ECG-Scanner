
#include <stdio.h>
#include <stdlib.h>
#include "filters.h"
#include "buffer.h"

void lowPassFilter(buff * input,buff * output){
	int yn1	= getHeadBuffer(output);
	int yn2 = getPreviousBuffer(1,output);
	int xn = getHeadBuffer(input);
	int xn6 = getPreviousBuffer(6,input);
	int xn12 = getPreviousBuffer(12,input);

	int yn;
	yn = 2*yn1-yn2+(xn-(2*xn6)+xn12)/32;

	insertToBuffer(yn,output);
}

void highPassFilter(buff * input,buff * output){
	int yn1 = getHeadBuffer(output);
	int xn = getHeadBuffer(input);
	int xn16 = getPreviousBuffer(16,input);
	int xn17 = getPreviousBuffer(17,input);
	int xn32 = getPreviousBuffer(32,input);

	int yn;
	yn = yn1-(xn/32)+xn16-xn17+(xn32/32);

	insertToBuffer(yn,output);
}

void derivativeFilter(buff * input,buff * output){
	int xn = getHeadBuffer(input);
	int xn1 = getPreviousBuffer(1,input);
	int xn3 = getPreviousBuffer(3,input);
	int xn4 = getPreviousBuffer(4,input);

	int yn;
	yn = (2*xn+xn1-xn3-2*xn4)/8;
	insertToBuffer(yn,output);

}

void squaringFilter(buff * input,buff * output){
	int xn = getHeadBuffer(input);
	int yn = xn*xn;
	insertToBuffer(yn,output);
}

void movingWindowFilter(buff * input,buff * output){
	int const N = 30;
	int i;
	int sum = 0;
	for(i = 0; i < N; i++){
		sum += getPreviousBuffer(i,input);
	}

	int yn = (sum/N);
	insertToBuffer(yn,output);
}


