#include "toyota.h"
#include "pal.h"
#include "stdio.h"
#include "flexcan.h"

DevCtrlOps toyota_ops;

CanTxMsg toyota_lamp_on = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
	0x40, 0x06, 0x30, 0x15, 0xff, 0x20, 0x00, 0x00
};

CanTxMsg toyota_lamp_off = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
	0x40, 0x06, 0x30, 0x15, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_door_on = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
	0x40, 0x05, 0x30, 0x11, 0x00, 0x80, 0x00, 0x00
};

CanTxMsg toyota_door_off = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
	0x40, 0x05, 0x30, 0x11, 0x00, 0x40, 0x00, 0x00
};

CanTxMsg toyota_trunk_on = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
	0x40, 0x05, 0x30, 0x11, 0xff, 0x00, 0x80, 0x00
};

CanTxMsg toyota_trunk_off = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
	0x40, 0x05, 0x30, 0x11, 0xff, 0x00, 0x00, 0x00
};

CanTxMsg toyota_sunfloor_on = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
	0xad, 0x04, 0x30, 0x01, 0x01, 0x40, 0x00, 0x00
};

CanTxMsg toyota_sunfloor_off = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
	0xad, 0x04, 0x30, 0x01, 0x01, 0x80, 0x00, 0x00
};

CanTxMsg toyota_window_on[4] = 
{
	{
		0x750, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x90, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
	},
	{
		0x750, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x91, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
	},
	{
		0x750, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x92, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
	},
	{
		0x750, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x93, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
	},
};

CanTxMsg toyota_window_off[4] = 
{
	{
		0x750, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x90, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
	},
	{
		0x750, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x91, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
	},
	{
		0x750, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x92, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
	},
	{
		0x750, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x93, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
	},
};

CanTxMsg toyota_keepalive_normal = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
	0x40, 0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_keepalive_door = 
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
	0x90, 0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00
};

void toyota_keepalive(void)
{
    flexcan_send_frame(&toyota_keepalive_normal);
    flexcan_send_frame(&toyota_keepalive_door);
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
    uint8_t i = 0;
    if(state) {
        for(i = 0; i < 4; i++) {
            flexcan_send_frame(&toyota_window_on[i]);
            OSTimeDlyHMSM(0, 0, 0, 500);
        }
    } else {
        for(i = 0; i < 4; i++) {
            flexcan_send_frame(&toyota_window_off[i]);
            OSTimeDlyHMSM(0, 0, 0, 500);
        }
    }
    for(i = 0; i < 3; i++) {
        toyota_keepalive();
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}

void toyota_ctrl_door(uint8_t state)
{
    toyota_keepalive();
    if(state) {
        flexcan_send_frame(&toyota_door_on);
    } else {
        flexcan_send_frame(&toyota_door_off);
    }
}

void toyota_ctrl_light(uint8_t state)
{
    toyota_keepalive();
    if(state) {
        flexcan_send_frame(&toyota_lamp_on);
    } else {
        flexcan_send_frame(&toyota_lamp_off);
    }
}

void toyota_ctrl_sunfloor(uint8_t state)
{
    toyota_keepalive();
    if(state) {
        flexcan_send_frame(&toyota_sunfloor_on);
    } else {
        flexcan_send_frame(&toyota_sunfloor_off);
    }
}

void toyota_ctrl_trunk(uint8_t state)
{
    toyota_keepalive();
    if(state) {
        flexcan_send_frame(&toyota_trunk_on);
    } else {
        flexcan_send_frame(&toyota_trunk_off);
    }

}

void toyota_ctrl_findcar(uint8_t state)
{
    printf("-> %s\r\n", __func__);
}

