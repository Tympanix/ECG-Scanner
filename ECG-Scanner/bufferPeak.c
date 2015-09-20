
#include <stdio.h>
#include <stdlib.h>
#include "bufferPeak.h"


void initBufferPeak(int size, buffPeak * buffer)
{
	buffer->data = calloc(size,sizeof(*(buffer->data)));
	buffer->head = size-1;
	buffer->size = size;
	buffer->inserts = 0;


}

void cleanupBufferPeak(buffPeak * buffer)
{
	free(buffer->data);
}

void insertToBufferPeak(Peak data, buffPeak * buffer){
	movePointerBufferPeak(buffer);
	buffer->data[buffer->head] = data;
	buffer->inserts++;
}

void movePointerBufferPeak(buffPeak * buffer){
	buffer->head += 1;
	if(buffer->head >= buffer->size){
		buffer->head = buffer->head%buffer->size;
	}
}

Peak getPreviousPeak(int previousN, buffPeak * buffer){
	int index = ((buffer->head+buffer->size)-(previousN%buffer->size))
			%buffer->size;
	return buffer->data[index];
}

Peak getHeadPeak(buffPeak * buffer){
	return getPreviousPeak(0,buffer);
}

int getAverageRRPeak(buffPeak * buffer, int goback) {
	int count = min((buffer->inserts)-1, goback);
	if (count == 0) return 0;
	int i;
	int sum = 0;
	for (i = 0; i < count; i++) {
		Peak now = getPreviousPeak(i, buffer);
		Peak before = getPreviousPeak(i + 1, buffer);
		sum += (now.time - before.time);
	}
	return sum / count;
}




