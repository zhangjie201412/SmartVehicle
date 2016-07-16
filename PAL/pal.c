#include "pal.h"
#include "stdio.h"
#include "fake.h"
#include "toyota.h"
#include "gm.h"
#include "transmit.h"
#include "wdg.h"
#include "flexcan.h"
#include "m25p16.h"

#define DEVICE_ID_ADDRESS               0x80
#define TRANSMIT_TASK_STK_SIZE          128
OS_STK transmitTaskStk[TRANSMIT_TASK_STK_SIZE];

#define IMMOLOCK_TASK_STK_SIZE          128
OS_STK immolockTaskStk[IMMOLOCK_TASK_STK_SIZE];

#define UPLOAD_TASK_STK_SIZE            256
OS_STK uploadTaskStk[UPLOAD_TASK_STK_SIZE];

PidItem pidList[PID_SIZE] =
{
    {ENG_DATA_RPM, "eng_data_rpm"},
    {ENG_DATA_VS, "eng_data_vs"},
    {ENG_DATA_ECT, "eng_data_ect"},
};

static void transmit_thread(void *parg);
static void immolock_thread(void *parg);
static void upload_thread(void *unused);
__IO uint8_t deviceid[17];

__IO static Pal mPal;

void pal_init(void)
{
    printf("-> %s\r\n", __func__);
    //create mailbox
    mPal.mailbox = OSMboxCreate((void *)0);
    
    transmit_init();
    iwdg_init(IWDG_Prescaler_256, 0xfff);
    OSTaskCreate(transmit_thread, (void *)0,
            &transmitTaskStk[TRANSMIT_TASK_STK_SIZE - 1],
            TRANSMIT_TASK_PRIO);
//    OSTaskCreate(immolock_thread, (void *)0,
//            &immolockTaskStk[IMMOLOCK_TASK_STK_SIZE - 1],
//            IMMOLOCK_TASK_PRIO);
    OSTaskCreate(upload_thread, (void *)0,
            &uploadTaskStk[UPLOAD_TASK_STK_SIZE - 1],
            UPLOAD_TASK_PRIO);
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
        pal_do_bcm(ctrlMsg->id, ctrlMsg->value, ctrlMsg->cmd_id);
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

void pal_do_bcm(uint8_t id, uint8_t val, uint32_t cmd_id)
{
    //printf("%s: ++id = %d, val = %d++\r\n", __func__, id, val);
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
    //response the cmd_id and cmd_type
    control_rsp(cmd_id, id);
    //printf("%s:----\r\n", __func__);
}

void upload_thread(void *unused)
{
    uint8_t i = 0, j;
    uint8_t *data;
    uint8_t len;

    unused = unused;
    //main thread for upload vehicle data
    for(;;) {
        OSTimeDlyHMSM(0, 0, 5, 0);
        for(i = 0; i < PID_SIZE; i++) {
            data = mPal.uploadOps->transfer_data_stream(i, &len);
            for(j = 0; j < len; j++) {
                printf("%d(%02x) ", data[j], data[j]);
            }
            printf("\r\n");
        }
    }
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
