
#include <stdio.h>
#include <stdlib.h>
#include "bufferPeak.h"




void initBufferPeak(int size, buffPeak * buffer)
{
	buffer->data = calloc(size,sizeof(*(buffer->data)));
	buffer->head = size-1;
	buffer->size = size;


}

void cleanupBufferPeak(buffPeak * buffer)
{
	free(buffer->data);
}

void insertToBufferPeak(Peak data, buffPeak * buffer){
	movePointerBufferPeak(buffer);
	buffer->data[buffer->head] = data;
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




