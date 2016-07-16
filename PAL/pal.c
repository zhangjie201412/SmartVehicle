#include "pal.h"
#include "stdio.h"
#include "fake.h"
#include "toyota.h"
#include "gm.h"
#include "transmit.h"
#include "wdg.h"
#include "flexcan.h"

#define DEVICE_ID_ADDRESS               0x80
#define TRANSMIT_TASK_STK_SIZE          128
OS_STK transmitTaskStk[TRANSMIT_TASK_STK_SIZE];

#define IMMOLOCK_TASK_STK_SIZE          128
OS_STK immolockTaskStk[IMMOLOCK_TASK_STK_SIZE];

static void transmit_thread(void *parg);
static void immolock_thread(void *parg);
__IO uint8_t deviceid[17];

Pal mPal;

void pal_init(void)
{
    uint8_t i = 0;
    CanRxMsg *rx;
    printf("-> %s\r\n", __func__);
    //create mailbox
    mPal.mailbox = OSMboxCreate((void *)0);
    
    transmit_init();
    iwdg_init(IWDG_Prescaler_256, 0xfff);
    OSTaskCreate(transmit_thread, (void *)0,
            &transmitTaskStk[TRANSMIT_TASK_STK_SIZE - 1],
            TRANSMIT_TASK_PRIO);
    OSTaskCreate(immolock_thread, (void *)0,
            &immolockTaskStk[IMMOLOCK_TASK_STK_SIZE - 1],
            IMMOLOCK_TASK_PRIO);
}

static void transmit_thread(void *pargs)
{
    INT8U err;
    CtrlMsg *ctrlMsg;

    pargs = pargs;

    while(1) {
        ctrlMsg = (CtrlMsg *)OSMboxPend(mPal.mailbox, 0, &err);
        printf("id = %d, cmd_id = %d, value = %d\r\n",
                ctrlMsg->id, ctrlMsg->cmd_id, ctrlMsg->value);
        pal_do_bcm(ctrlMsg->id, ctrlMsg->value);
    }
}

uint8_t immo_state = 0;

void set_immo_state(uint8_t state) {
    immo_state = state;
    //printf("%s: state = %d\r\n", __func__, immo_state);
}

static void immolock_thread(void *parg)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    GPIO_ResetBits(GPIOC, GPIO_Pin_14);

    parg = parg;

    printf("immo thread start\r\n");
    while(1) {
        if(immo_state) {
            immolock(1);
        } else {
            immolock(0);
        }
        OSTimeDlyHMSM(0, 0, 0, 50);
    }
}

void immolock(uint8_t state)
{
    uint16_t i = 0;

    //printf("state = %d\r\n", state);
    for(i = 0; i < 300; i++) {
        if(state == 0) {
            GPIO_SetBits(GPIOC, GPIO_Pin_14);
            delay(8);
            GPIO_ResetBits(GPIOC, GPIO_Pin_14);
            delay(2);
        }
    }
}

void pal_do_bcm(uint8_t id, uint8_t val)
{
    printf("%s: ++id = %d, val = %d++\r\n", __func__, id, val);
    switch(id) {
        case CONTROL_IMMOLOCK:
            set_immo_state(val);
            break;
        case CONTROL_WINDOW:
            mPal.ops->control_window(val);
            break;
        case CONTROL_DOOR:
            mPal.ops->control_door(val);
            break;
        case CONTROL_LIGHT:
            mPal.ops->control_light(val);
            break;
        case CONTROL_SUNFLOOR:
            mPal.ops->control_sunfloor(val);
            break;
        case CONTROL_TRUNK:
            mPal.ops->control_trunk(val);
            break;
        case CONTROL_FINDCAR:
            mPal.ops->control_findcar(val);
            break;

        default:
            printf("invalid cmd id\r\n");
            break;
    }
    printf("%s:----\r\n", __func__);
}

void getDeviceId(void)
{
    //read device id from flash
    flash_page_read(deviceid, DEVICE_ID_ADDRESS, 17);
}

Pal *getPalInstance(void)
{
    return &mPal;
}
