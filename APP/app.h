#ifndef __APP_H__
#define __APP_H__

//5 min
#define CONNECTION_TIMEOUT      300
#define PROP_HAS_GPS            1
#define PROP_HAS_GPRS           1
#define PROP_HAS_RF             1

void go_reboot(void);
uint32_t getTicks(void);

#endif
