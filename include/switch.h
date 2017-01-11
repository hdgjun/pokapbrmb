/*
 * Switch.h
 *
 *  Created on: 2016年12月23日
 *      Author: Administrator
 */

#ifndef _INCLUDE_SWITCH_H_
#define _INCLUDE_SWITCH_H_

#ifndef _LIBC_REENTRANT
#define _LIBC_REENTRANT     /*errno是多线程/进程安全*/
#endif

//#define DB_ORACLE  /*编译oralce*/
//#define DB_MYSQL  /*编译mysql*/
#define TIME_FLAG   /*coltime使用文件名时间，注释后使用真实coltime*/
#define BFNEW       /*定义则为新bffile格式*/
//#define DEBUG    /*调试模式,打印所有日志*/
//#define PEOPLEBANK   /*定义走人行流程  注释走商行流程*/

#endif /* INCLUDE_SWITCH_H_ */
