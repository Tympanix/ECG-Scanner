#include <stdio.h>
#include <stdlib.h>
#include "dynamicList.h"
#include "peak.h"

void initList(DynamicList * list)
{
	list->size = 10;
	list->data = calloc(list->size,sizeof(*(list->data)));
	list->head = -1;
}

static void doubleListSize(DynamicList * list){
	int newSize = 2*list->size;
	Peak * newData = calloc(newSize,sizeof(*(list->data)));
	int i;
	for(i = 0; i < list->size; i++){
		newData[i] = list->data[i];
	}
	list->size = newSize;
	free(list->data);
	list->data = newData;
}

void insertToList(int value, int time, DynamicList * list){
	Peak peak = {value, time};
	if(list->head >= list->size-1){
		doubleListSize(list);
	}
	list->head++;
	list->data[list->head] = peak;
}

void cleanupList(DynamicList * list){
	free(list->data);
}

Peak getIndexList(int index, DynamicList * list){
	return list->data[index];
}

Peak getHeadList(DynamicList * list){
	//if?
	return list->data[list->head];
}

void toStringList(DynamicList * list){
	int i;
	for(i = 0; i <= list->head; i++){
		printf("[%d, %d], ", (list->data[i]).value,(list->data[i]).time);
	}
	printf("size: %d, head: %d\n" , list->size, list->head);
}

