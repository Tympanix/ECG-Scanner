
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "filters.h"
#include "buffer.h"
#include "buffer64bit.h"

const int OVERFLOW_HIGH_LIMIT = (INT_MAX/4)*3;
const int OVERFLOW_LOW_LIMIT = (INT_MIN/4)*3;

void lowPassFilter(buff * input, buff * output){
	long long int yn1	= getHeadBuffer64bit(output);
	long long int yn2 = getPreviousBuffer64bit(1,output);
	long long int xn = (long long) getHeadBuffer(input);
	long long int xn6 = (long long) getPreviousBuffer(6, input);
	long long int xn12 = (long long) getPreviousBuffer(12, input);

	long long int yn;
	yn = 2*yn1-yn2+(xn-(2*xn6)+xn12)/32;

	insertToBuffer64bit(yn, output);
}

void highPassFilter(buff64bit * input,buff * output){
	long long int yn1 = (long long) getHeadBuffer(output);
	long long int xn = getHeadBuffer64bit(input);
	long long int xn16 = getPreviousBuffer64bit(16,input);
	long long int xn17 = getPreviousBuffer64bit(17,input);
	long long int xn32 = getPreviousBuffer64bit(32,input);

	int yn;
	yn = (int) (yn1-(xn/32)+xn16-xn17+(xn32/32));

	insertToBuffer(yn, output);
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



