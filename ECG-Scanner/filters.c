
#include <stdio.h>
#include <stdlib.h>
#include "filters.h"
#include "buffer.h"

void lowPassFilter(buff * input,buff * output){
	int yn1	= getHead(output);
	int yn2 = getPrevious(1,output);
	int xn = getHead(input);
	int xn6 = getPrevious(6,input);
	int xn12 = getPrevious(12,input);

	int yn;
	yn = 2*yn1-yn2+(xn-(2*xn6)+xn12)/32;

	insertToBuffer(yn,output);
}

void highPassFilter(buff * input,buff * output){
	int yn1 = getHead(output);
	int xn = getHead(input);
	int xn16 = getPrevious(16,input);
	int xn17 = getPrevious(17,input);
	int xn32 = getPrevious(32,input);

	int yn;
	yn = yn1-(xn/32)+xn16-xn17+(xn32/32);

	insertToBuffer(yn,output);
}

void derivativeFilter(buff * input,buff * output){
	int xn = getHead(input);
	int xn1 = getPrevious(1,input);
	int xn3 = getPrevious(3,input);
	int xn4 = getPrevious(4,input);

	int yn;
	yn = (2*xn+xn1-xn3-2*xn4)/8;
	insertToBuffer(yn,output);

}

void squaringFilter(buff * input,buff * output){
	int xn = getHead(input);
	int yn = xn*xn;
	insertToBuffer(yn,output);
}

void movingWindowFilter(buff * input,buff * output){
	int const N = 30;
	int i;
	int sum = 0;
	for(i = 0; i < N; i++){
		sum += getPrevious(i,input);
	}

	int yn = (sum/N);
	insertToBuffer(yn,output);
}


