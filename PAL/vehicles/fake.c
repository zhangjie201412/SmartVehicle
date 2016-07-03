#include "fake.h"
#include "pal.h"
#include "stdio.h"

DevCtrlOps fake_ops;

void fake_setup(void)
{
    Pal *pal;
    printf("-> %s\r\n", __func__);
    pal = getPalInstance();
    //init fake ops
    fake_ops.control_door = fake_ctrl_door;
    fake_ops.control_window = fake_ctrl_window;
    fake_ops.control_sunfloor = fake_ctrl_sunfloor;
    fake_ops.control_light = fake_ctrl_light;
    fake_ops.control_findcar = fake_ctrl_findcar;
    fake_ops.control_trunk = fake_ctrl_trunk;

    pal->ops = &fake_ops;
}

void fake_ctrl_window(uint8_t state)
{
    printf("-> %s\r\n", __func__);
}

void fake_ctrl_door(uint8_t state)
{
    printf("-> %s\r\n", __func__);
}

void fake_ctrl_light(uint8_t state)
{
    printf("-> %s\r\n", __func__);

}

void fake_ctrl_sunfloor(uint8_t state)
{
    printf("-> %s\r\n", __func__);
    
}

void fake_ctrl_trunk(uint8_t state)
{
    printf("-> %s\r\n", __func__);

}

void fake_ctrl_findcar(uint8_t state)
{
    printf("-> %s\r\n", __func__);
    
}

