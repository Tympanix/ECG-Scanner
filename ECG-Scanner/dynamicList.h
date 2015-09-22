/*
 * dynamicList.h
 *
 *  Created on: 16 Sep 2015
 *      Author: feynman
 */

#ifndef DYNAMICLIST_H_
#define DYNAMICLIST_H_
#include "peak.h"

typedef struct{
	int size;
	int head;
	Peak * data;
}DynamicList;

void initList(DynamicList * list);
static void doubleListSize(DynamicList * list);
void insertToList(int value, int time, DynamicList * list);
void cleanupList(DynamicList * list);
Peak getIndexList(int index, DynamicList * list);
Peak getHeadList(DynamicList * list);
void toStringList(DynamicList * list);

#endif /* DYNAMICLIST_H_ */
