
#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"




void initBuffer(int size, buff * buffer)
{
	buffer->data = calloc(size,sizeof(*(buffer->data)));
	buffer->head = size-1;
	buffer->size = size;
}

void cleanupBuffer(buff * buffer)
{
	free(buffer->data);
}

void insertToBuffer(int data, buff * buffer){
	buffer->inserts++;
	movePointerBuffer(buffer);
	buffer->data[buffer->head] = data;
}

void movePointerBuffer(buff * buffer){
	buffer->head += 1;
	if(buffer->head >= buffer->size){
		buffer->head = buffer->head%buffer->size;
	}
}

int getPreviousBuffer(int previousN, buff * buffer){
	int index = ((buffer->head+buffer->size)-(previousN%buffer->size))
			%buffer->size;
	return buffer->data[index];
}

int getHeadBuffer(buff * buffer){
	return getPreviousBuffer(0,buffer);
}

int getAvgBuffer(buff * buffer) {
	int count = buffer->size;
	int actual = 0;
	int sum = 0;
	int i;

	for (i = 0; i < count; i++) {
		int add = getPreviousBuffer(i, buffer);
		if (add == 0) break;
		sum += add;
		actual++;
	}

	if (actual == 0) return 0;
	return sum / actual;
}




