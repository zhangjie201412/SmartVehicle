/*
 *********************************************************************************************************
 *                                              EXAMPLE CODE
 *
 *                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
 *
 *               All rights reserved.  Protected by international copyright laws.
 *               Knowledge of the source code may NOT be used to develop a similar product.
 *               Please help us continue to provide the Embedded community with the finest
 *               software available.  Your honesty is greatly appreciated.
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *
 *                                      APPLICATION CONFIGURATION
 *
 *                                     ST Microelectronics STM32
 *                                              with the
 *                                   STM3210B-LK1 Evaluation Board
 *
 * Filename      : app_cfg.h
 * Version       : V1.10
 * Programmer(s) : BH3NVN
 *********************************************************************************************************
 */

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
 *********************************************************************************************************
 *                                       MODULE ENABLE / DISABLE
 *********************************************************************************************************
 */


/*
 *********************************************************************************************************
 *                                              TASKS NAMES
 *********************************************************************************************************
 */


/*
 *********************************************************************************************************
 *                                            TASK PRIORITIES
 *********************************************************************************************************
 */

#define  APP_TASK_START_PRIO                    2
#define  IMMOLOCK_TASK_PRIO                     3
//process at command
#define task_process_atcmd_prio                 10
//work process
#define task_report_work_prio                   11
#define SIM900_RX_PRIO                          5
#define FAKE_TASK_PRIO                          6
#define TRANSMIT_TASK_PRIO                      7
#define HEARTBEAT_THREAD_PRIO                   13
#define UPLOAD_TASK_PRIO                        8

#define TASK_STK_SIZE_COMMON        128

#define  APP_TASK_START_STK_SIZE		128
//process at command
#define task_process_atcmd_stk_size		256
//work process
#define task_report_work_stk_size       128

/*
 *********************************************************************************************************
 *                                                  LIB
 *********************************************************************************************************
 */

#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED

#endif
