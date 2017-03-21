/*
 * dbtbundle.h
 *
 *  Created on: 2017Äê2ÔÂ22ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DB_DBTBUNDLE_H_
#define INCLUDE_DB_DBTBUNDLE_H_
#include "pokaqueue.h"

typedef struct T_Bundle
{
	char bundleCode[30+1];
	char orderId[30+1];
}TBUNDLE,pTbundle;

int DbTBundleGetCount(int *cSize, char *order);
int DbTBundleByOrder(cir_queue_t *q, char *order);
int DbTRSBundleGetCount(int *cSize, char *order);
int DbTRSBundleByOrder(cir_queue_t *q, char *order);
#endif /* INCLUDE_DB_DBTBUNDLE_H_ */
