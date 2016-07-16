#include "utils.h"
#include "flexcan.h"

void sendStdTxMsg(StdTxMsg *msg)
{
    uint8_t i;
    CanTxMsg tx;

    tx.StdId = msg->id;
    tx.DLC = msg->len;
    tx.IDE = CAN_ID_STD;

    for(i = 0; i < tx.DLC; i++) {
        tx.Data[i] = msg->data[i];
    }
    //delay 200ms for response
    OSTimeDlyHMSM(0, 0, 0, 100);
    flexcan_send_frame2(&tx);
    //flexcan_ioctl(DIR_OUTPUT, &tx, 0, 0);
    //++for debug
    printf("++id = %04x, len = %d++\r\n", msg->id, msg->len);
    for(i = 0; i < msg->len; i++) {
        printf("%02x  ", msg->data[i]);
    }
    printf("\r\n");
}
