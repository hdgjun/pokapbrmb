/*
 * datecut.h
 *
 *  Created on: 2016��12��27��
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
