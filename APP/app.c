#define GLOBALS

#include "stdarg.h"

#include "includes.h"
#include "globals.h"
#include "app.h"

#include "m25p16.h"
#include "sim900.h"
#include "gps.h"
#include "cJSON.h"
#include "flexcan.h"
#include "wdg.h"
#include "pal.h"

OS_EVENT* atCmdMailbox;
int isRunning = 0;
uint8_t engineOn = 0;
//unit = second
__IO uint32_t ticks = 0;

/*
 *********************************************************************************************************
 *                                            LOCAL DEFINES
 *********************************************************************************************************
 */



/*
 *********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 *********************************************************************************************************
 */

static OS_STK App_TaskStartStk[APP_TASK_START_STK_SIZE];
//process at command
static OS_STK task_process_atcmd_stk[task_process_atcmd_stk_size];

/*
 *********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************
 */
static  void App_TaskCreate(void);
static  void App_TaskStart(void* p_arg);
//process atcmd task
static void task_process_atcmd(void *parg);

int main(void)
{
    CPU_INT08U os_err;
    CPU_IntDis();
    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel".         */
    BSP_Init();                                                 /* Initialize BSP functions.  */
    printk("Vehicle Union device Start!\r\n");
    os_err = OSTaskCreate((void (*) (void *)) App_TaskStart,	  		  		//指向任务代码的指针
            (void *) 0,								  		//任务开始执行时，传递给任务的参数的指针
            (OS_STK *) &App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],	//分配给任务的堆栈的栈顶指针   从顶向下递减
            (INT8U) APP_TASK_START_PRIO);								//分配给任务的优先级

    if(os_err != OS_ERR_NONE) {
        printk("create main task failed\r\n");
    }
    OSTimeSet(0);
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */

    return (0);
}

/*
 *********************************************************************************************************
 *                                          App_TaskStart()
 *
 * Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
 *
 * Argument : p_arg       Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
 *
 * Return   : none.
 *
 * Caller   : This is a task.
 *
 * Note     : none.
 *********************************************************************************************************
 */

static uint8_t need_reboot = 0;

void go_reboot(void)
{
    printk("####REBOOT####\r\n");
    need_reboot = 1;
}

uint32_t getTicks(void) {
    return ticks;
}

void setTicks(uint32_t t) {
    ticks = t;
}

static  void App_TaskStart(void* p_arg)
{
    (void) p_arg;

    OS_CPU_SysTickInit();                                       /* Initialize the SysTick.       */
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity.                              */
#endif
    App_TaskCreate();

    need_reboot = 0;
    while (1)
    {
        OSTimeDlyHMSM(0, 0, 1, 0);
        if(need_reboot == 0)
            iwdg_feed();
        ticks += 1;
#ifdef PROP_HAS_GPRS
        if(ticks >= CONNECTION_TIMEOUT) {
            go_reboot();
        }
#endif
        immo_process();
    }
}

/*
 *********************************************************************************************************
 *                                            App_TaskCreate()
 *
 * Description : Create the application tasks.
 *
 * Argument : none.
 *
 * Return   : none.
 *
 * Caller   : App_TaskStart().
 *
 * Note     : none.
 *********************************************************************************************************
 */

static  void App_TaskCreate(void)
{
    OSTaskCreateExt(task_process_atcmd,
            (void *)0,
            (OS_STK *)&task_process_atcmd_stk[task_process_atcmd_stk_size - 1],
            task_process_atcmd_prio,
            task_process_atcmd_prio,
            (OS_STK *)&task_process_atcmd_stk[0],
            task_process_atcmd_stk_size,
            (void *)0,
            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
}

static void task_process_atcmd(void *parg)
{
    (void)parg;

    printk("%s: Enter\r\n", __func__);
    flash_init();
    flexcan_init(CAN_500K);
    flexcan_filter(0x641, 0x641, 0x6ff, 0x6ff);

    pal_init();
    while(1) {
        OSTimeDlyHMSM(0, 0, 5, 0);
    }
}

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                          uC/OS-II APP HOOKS
 *********************************************************************************************************
 *********************************************************************************************************
 */

#if (OS_APP_HOOKS_EN > 0)
/*
 *********************************************************************************************************
 *                                      TASK CREATION HOOK (APPLICATION)
 *
 * Description : This function is called when a task is created.
 *
 * Argument : ptcb   is a pointer to the task control block of the task being created.
 *
 * Note     : (1) Interrupts are disabled during this call.
 *********************************************************************************************************
 */

void App_TaskCreateHook(OS_TCB* ptcb)
{
}

/*
 *********************************************************************************************************
 *                                    TASK DELETION HOOK (APPLICATION)
 *
 * Description : This function is called when a task is deleted.
 *
 * Argument : ptcb   is a pointer to the task control block of the task being deleted.
 *
 * Note     : (1) Interrupts are disabled during this call.
 *********************************************************************************************************
 */

void App_TaskDelHook(OS_TCB* ptcb)
{
    (void) ptcb;
}

/*
 *********************************************************************************************************
 *                                      IDLE TASK HOOK (APPLICATION)
 *
 * Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
 *               has been added to allow you to do such things as STOP the CPU to conserve power.
 *
 * Argument : none.
 *
 * Note     : (1) Interrupts are enabled during this call.
 *********************************************************************************************************
 */

#if OS_VERSION >= 251
void App_TaskIdleHook(void)
{
}
#endif

/*
 *********************************************************************************************************
 *                                        STATISTIC TASK HOOK (APPLICATION)
 *
 * Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
 *               statistics task.  This allows your application to add functionality to the statistics task.
 *
 * Argument : none.
 *********************************************************************************************************
 */

void App_TaskStatHook(void)
{
}

/*
 *********************************************************************************************************
 *                                        TASK SWITCH HOOK (APPLICATION)
 *
 * Description : This function is called when a task switch is performed.  This allows you to perform other
 *               operations during a context switch.
 *
 * Argument : none.
 *
 * Note     : 1 Interrupts are disabled during this call.
 *
 *            2  It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
 *                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
 *                  task being switched out (i.e. the preempted task).
 *********************************************************************************************************
 */

#if OS_TASK_SW_HOOK_EN > 0
void App_TaskSwHook(void)
{
}
#endif

/*
 *********************************************************************************************************
 *                                     OS_TCBInit() HOOK (APPLICATION)
 *
 * Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
 *               up most of the TCB.
 *
 * Argument : ptcb    is a pointer to the TCB of the task being created.
 *
 * Note     : (1) Interrupts may or may not be ENABLED during this call.
 *********************************************************************************************************
 */

#if OS_VERSION >= 204
void App_TCBInitHook(OS_TCB* ptcb)
{
    (void) ptcb;
}
#endif

#endif
