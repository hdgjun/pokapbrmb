/*
 * datecut.h
 *
 *  Created on: 2016Äê12ÔÂ27ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_DATECUT_H_
#define INCLUDE_DATECUT_H_

#include <semaphore.h>

void destroy_date_cut();
int init_date_cut();
int is_date_cut();
int update_date_cup();
void *date_cut_thread();
#endif /* INCLUDE_DATECUT_H_ */
