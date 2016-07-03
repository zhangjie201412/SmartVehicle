#include "pal.h"
#include "stdio.h"
#include "fake.h"
#include "transmit.h"
#include "wdg.h"

#define DEVICE_ID_ADDRESS               0x80
#define TRANSMIT_TASK_STK_SIZE          128
OS_STK transmitTaskStk[TRANSMIT_TASK_STK_SIZE];
static void transmit_thread(void *parg);
__IO uint8_t deviceid[17];

Pal mPal;

void pal_init(void)
{
    printf("-> %s\r\n", __func__);
    //create mailbox
    mPal.mailbox = OSMboxCreate((void *)0);
    fake_setup();
    transmit_init();
    iwdg_init(IWDG_Prescaler_256, 0xfff);
    OSTaskCreate(transmit_thread, (void *)0,
            &transmitTaskStk[TRANSMIT_TASK_STK_SIZE - 1],
            TRANSMIT_TASK_PRIO);
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

void pal_do_bcm(uint8_t id, uint8_t val)
{
    switch(id) {
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
