#include "toyota.h"
#include "pal.h"
#include "stdio.h"
#include "flexcan.h"

DevCtrlOps toyota_ops;

CanTxMsg keepalive =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg keepalive_door =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x90, 0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00
};

void toyota_keepalive(void)
{
}

void toyota_setup(void)
{
    Pal *pal;
    printf("-> %s\r\n", __func__);
    pal = getPalInstance();
    //init toyota ops
    toyota_ops.control_door = toyota_ctrl_door;
    toyota_ops.control_window = toyota_ctrl_window;
    toyota_ops.control_sunfloor = toyota_ctrl_sunfloor;
    toyota_ops.control_light = toyota_ctrl_light;
    toyota_ops.control_findcar = toyota_ctrl_findcar;
    toyota_ops.control_trunk = toyota_ctrl_trunk;

    pal->ops = &toyota_ops;
}

void toyota_ctrl_window(uint8_t state)
{
    CanTxMsg msg = 
    {
        0x7df, 0x00,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0x01, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    printf("-> %s\r\n", __func__);
    flexcan_ioctl(DIR_OUTPUT, &msg, 0x7e8, 1);
}

void toyota_ctrl_door(uint8_t state)
{
    printf("-> %s\r\n", __func__);
}

void toyota_ctrl_light(uint8_t state)
{
    printf("-> %s\r\n", __func__);

}

void toyota_ctrl_sunfloor(uint8_t state)
{
    printf("-> %s\r\n", __func__);
    
}

void toyota_ctrl_trunk(uint8_t state)
{
    printf("-> %s\r\n", __func__);

}

void toyota_ctrl_findcar(uint8_t state)
{
    printf("-> %s\r\n", __func__);
    
}

