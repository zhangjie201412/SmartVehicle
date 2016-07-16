#include "gm.h"
#include "pal.h"
#include "stdio.h"
#include "utils.h"

DevCtrlOps gm_ops;

CanTxMsg gm_lamp_on = 
{
    0x241, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x07, 0xae, 0x02, 0x00, 0x00, 0x02, 0x02, 0x00
};

CanTxMsg gm_lamp_off = 
{
    0x241, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x07, 0xAE, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00
};

CanTxMsg gm_door_on[5] = {
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x01, 0x04, 0x04, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x01, 0x02, 0x02, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	},
};

CanTxMsg gm_door_off[5] = {
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	},
};

CanTxMsg gm_window_on[2] = {
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x3B, 0x0F, 0x03, 0x03, 0x03, 0x03
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x3B, 0x0F, 0x03, 0x03, 0x03, 0x03
	},
};

CanTxMsg gm_window_off[2] = {
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x3B, 0x0F, 0x02, 0x02, 0x02, 0x02
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x3B, 0x0F, 0x02, 0x02, 0x02, 0x02
	},
};

CanTxMsg gm_trunk_on[2] = {
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x01, 0x10, 0x10, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00
	},
};

CanTxMsg gm_trunk_off[2] = {
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	},
};

CanTxMsg gm_sunfloor_on[3] = {
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x4C, 0xFF, 0x08, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x4C, 0xFF, 0x08, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x4C, 0xFF, 0x08, 0x00, 0x00, 0x00
	},
};

CanTxMsg gm_sunfloor_off[3] = {
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x4C, 0xFF, 0x04, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x4C, 0xFF, 0x04, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x4C, 0xFF, 0x04, 0x00, 0x00, 0x00
	},
};

CanTxMsg gm_findcar_on[2] = {
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x10, 0x01, 0x01, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x07, 0xAE, 0x02, 0xF0, 0xF0, 0x00, 0x00, 0x00
	},
};

CanTxMsg gm_findcar_off[2] = {
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	},
	{
		0x241, 0x18db33f1,
		CAN_ID_STD, CAN_RTR_DATA,
		8,
		0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	},
};



CanTxMsg gm_keepalive = 
{
    0x241, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void gm_setup(void)
{
    Pal *pal;
    printf("-> %s\r\n", __func__);
    pal = getPalInstance();
    //init gm ops
    gm_ops.control_door = gm_ctrl_door;
    gm_ops.control_window = gm_ctrl_window;
    gm_ops.control_sunfloor = gm_ctrl_sunfloor;
    gm_ops.control_light = gm_ctrl_light;
    gm_ops.control_findcar = gm_ctrl_findcar;
    gm_ops.control_trunk = gm_ctrl_trunk;

    pal->ops = &gm_ops;
}

void gm_ctrl_window(uint8_t state)
{
    uint8_t i = 0;

    printf("-> %s\r\n", __func__);
    if(state) {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame2(&gm_window_on[i]);
            OSTimeDlyHMSM(0, 0, 5, 0);
        }
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame2(&gm_window_off[i]);
            OSTimeDlyHMSM(0, 0, 5, 0);
        }
    }
}

void gm_ctrl_door(uint8_t state)
{
    uint8_t i = 0;
    printf("-> %s\r\n", __func__);
    flexcan_send_frame2(&gm_keepalive);
    OSTimeDlyHMSM(0, 0, 1, 0);
    if(state) {
        for(i = 0; i < 5; i++) {
            flexcan_send_frame2(&gm_door_on[i]);
        }
    } else {
        for(i = 0; i < 5; i++) {
            flexcan_send_frame2(&gm_door_off[i]);
        }
    }
    flexcan_send_frame2(&gm_keepalive);
}

void gm_ctrl_light(uint8_t state)
{
    printf("-> %s\r\n", __func__);
    flexcan_send_frame2(&gm_keepalive);
    OSTimeDlyHMSM(0, 0, 1, 0);
    if(state) {
        flexcan_send_frame2(&gm_lamp_on);
    } else {
        flexcan_send_frame2(&gm_lamp_off);
    }
    OSTimeDlyHMSM(0, 0, 1, 0);
    flexcan_send_frame2(&gm_keepalive);
}

void gm_ctrl_sunfloor(uint8_t state)
{
    uint8_t i = 0;
    printf("-> %s\r\n", __func__);
    if(state) {
        for(i = 0; i < 3; i ++) {
            flexcan_send_frame2(&gm_sunfloor_on[i]);
            OSTimeDlyHMSM(0, 0, 3, 0);
        }
    } else {
        for(i = 0; i < 3; i++) {
            flexcan_send_frame2(&gm_sunfloor_off[i]);
            OSTimeDlyHMSM(0, 0, 3, 0);
        }
    }
}

void gm_ctrl_trunk(uint8_t state)
{
    uint8_t i = 0;
    printf("-> %s\r\n", __func__);
    flexcan_send_frame2(&gm_keepalive);
    OSTimeDlyHMSM(0, 0, 1, 0);
    if(state) {
        for(i = 0; i < 2; i ++) {
            flexcan_send_frame2(&gm_trunk_on[i]);
            OSTimeDlyHMSM(0, 0, 0, 500);
        }
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame2(&gm_trunk_off[i]);
            OSTimeDlyHMSM(0, 0, 0, 500);
        }
    }
    flexcan_send_frame2(&gm_keepalive);
}

void gm_ctrl_findcar(uint8_t state)
{
    uint8_t i = 0;
    printf("-> %s\r\n", __func__);
    flexcan_send_frame2(&gm_keepalive);
    OSTimeDlyHMSM(0, 0, 1, 0);
    if(state) {
        for(i = 0; i < 2; i ++) {
            flexcan_send_frame2(&gm_findcar_on[i]);
            OSTimeDlyHMSM(0, 0, 0, 200);
        }
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame2(&gm_findcar_off[i]);
            OSTimeDlyHMSM(0, 0, 0, 200);
        }
    }
    flexcan_send_frame2(&gm_keepalive);    
}

