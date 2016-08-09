#include "gps.h"
#include "sim900.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "app.h"
#include "..\..\APP\includes.h"

void gps_init(void)
{
    u8 data;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    if (GPS_USARTAPB == APB1)
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_GPS_USART_IO | RCC_APB2Periph_AFIO,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APBxPeriph_GPS_USART,ENABLE);
    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_GPS_USART_IO | RCC_APBxPeriph_GPS_USART | RCC_APB2Periph_AFIO,ENABLE);
    }
    if (GPS_PinRemap == ENABLE)
    {
        GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
    }
    GPIO_InitStructure.GPIO_Pin = GPS_USART_TXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPS_USART_IO,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPS_USART_RXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPS_USART_IO,&GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_Init(GPS_USART,&USART_InitStructure);
    data = data;
    data = GPS_USART->DR;
    data = GPS_USART->SR;
    USART_ITConfig(GPS_USART,USART_IT_RXNE,ENABLE);
    USART_Cmd(GPS_USART,ENABLE);
}

void gps_send(uint8_t c)
{
    USART_SendData(GPS_USART, c);
    while(USART_GetFlagStatus(GPS_USART, USART_FLAG_TXE) == RESET)
    {;}
}

void gps_write(uint8_t *buf, uint16_t size)
{
    uint16_t i = 0;
    for(; i < size; i++) {
        gps_send(buf[i]);
    }
}

void gps_setup(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    printf("-> %s\r\n", __func__);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_12);
}

void gps_test(void)
{
    nmea_msg msg;
    uint8_t test1[128] = "$GNGGA,000024.000,3123.212191,N,12102.993440,E,0,00,127.000,20.506,M,0,M,,*56";
    uint8_t test2[128] = "$GNRMC,235947.000,V,3123.212191,N,12102.993440,E,0.000,0.000,,,E,N*2B";

    printf("-> %s\r\n", __func__);
    GPS_Analysis(&msg, test2);
    printf("lon = %d\r\n", msg.longitude);
    printf("lat = %d\r\n", msg.latitude);
}

__IO uint8_t rx_buf[100];
__IO uint8_t rx_len = 0;
__IO uint8_t last_byte;
__IO nmea_msg gpsx_msg;
uint32_t longitude = 0;
uint32_t latitude = 0;
uint32_t lon[10];
uint32_t lat[10];
uint8_t gps_index = 0;

uint32_t getLongitude(void)
{
    return longitude;
}

uint32_t getLatitude(void)
{
    return latitude;
}

void gps_process(uint8_t data)
{
    uint8_t i;
    char *tmp;
    uint32_t lat_tmp;
    uint32_t lon_tmp;
    //printf("%c", data);
    if(last_byte == 0x0d && (data == 0x0a)) {
        tmp = strstr((char *)rx_buf, "$GNRMC");
        if(tmp) {
            NMEA_GPRMC_Analysis(&gpsx_msg, rx_buf);
/*            printf("year = %d, mon = %d, date = %d, hour = %d, min = %d, sec = %d\r\n",
                    gpsx_msg.utc.year,
                    gpsx_msg.utc.month,
                    gpsx_msg.utc.date,
                    gpsx_msg.utc.hour,
                    gpsx_msg.utc.min,
                    gpsx_msg.utc.sec);
                    */
            lon[gps_index] = gpsx_msg.longitude;
            lat[gps_index] = gpsx_msg.latitude;
            gps_index ++;
            if(gps_index == 10) {
                gps_index = 0;
                lat_tmp = 0;
                lon_tmp = 0;
                //get the data
                for(i = 0; i < 10; i++) {
                    lat_tmp += lat[i];
                    lon_tmp += lon[i];
                }
                latitude = lat_tmp / 10;
                longitude = lon_tmp / 10;
            }
        }
        rx_len = 0;
        //memset(rx_buf, 0x00, 100);
        for(i = 0; i < 100; i++) {
            rx_buf[i] = 0x00;
        }
    }

    rx_buf[rx_len ++] = data;
    last_byte = data;
}

void GPS_USART_IRQHandler(void)
{
    uint8_t data = 0;

    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting ++;
    OS_EXIT_CRITICAL();

    if(USART_GetITStatus(GPS_USART, USART_IT_RXNE)
            != RESET) {
        data = USART_ReceiveData(GPS_USART);
        gps_process(data);
    }

    if(USART_GetITStatus(GPS_USART, USART_IT_TXE)
            != RESET) {
        USART_ITConfig(GPS_USART, USART_IT_TXE, DISABLE);
    }
    OSIntExit();
}

//////////////////////////////////////////////////////////////////////////////////
//NEO-6M GPSģ����������
//�޸�����:2014/2/8
//�汾��V2.0
//********************************************************************************
//V2.0 �޸�˵�� 20140208
//1,���Ublox_Cfg_Cfg_Save����
//2,���Ublox_Cfg_Msg����
//3,���Ublox_Cfg_Prt����.
//////////////////////////////////////////////////////////////////////////////////

//��buf����õ���cx���������ڵ�λ��
//����ֵ:0~0XFE,����������λ�õ�ƫ��.
//       0XFF,�������ڵ�cx������
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{
    u8 *p=buf;
    while(cx)
    {
        if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������
        if(*buf==',')cx--;
        buf++;
    }
    return buf-p;
}
//m^n����
//����ֵ:m^n�η�.
u32 NMEA_Pow(u8 m,u8 n)
{
    u32 result=1;
    while(n--)result*=m;
    return result;
}
//strת��Ϊ����,��','����'*'����
//buf:���ִ洢��
//dx:С����λ��,���ظ����ú���
//����ֵ:ת�������ֵ
int NMEA_Str2num(u8 *buf,u8*dx)
{
    u8 *p=buf;
    u32 ires=0,fres=0;
    u8 ilen=0,flen=0,i;
    u8 mask=0;
    int res;
    while(1) //�õ�������С���ĳ���
    {
        if(*p=='-'){mask|=0X02;p++;}//�Ǹ���
        if(*p==','||(*p=='*'))break;//����������
        if(*p=='.'){mask|=0X01;p++;}//����С������
        else if(*p>'9'||(*p<'0'))	//�зǷ��ַ�
        {
            ilen=0;
            flen=0;
            break;
        }
        if(mask&0X01)flen++;
        else ilen++;
        p++;
    }
    if(mask&0X02)buf++;	//ȥ������
    for(i=0;i<ilen;i++)	//�õ�������������
    {
        ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
    }
    if(flen>5)flen=5;	//���ȡ5λС��
    *dx=flen;	 		//С����λ��
    for(i=0;i<flen;i++)	//�õ�С����������
    {
        fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
    }
    res=ires*NMEA_Pow(10,flen)+fres;
    if(mask&0X02)res=-res;
    return res;
}
//����GPGSV��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
    u8 *p,*p1,dx;
    u8 len,i,j,slx=0;
    u8 posx;
    p=buf;
    p1=(u8*)strstr((const char *)p,"$GNGSV");
    len=p1[7]-'0';								//�õ�GPGSV������
    posx=NMEA_Comma_Pos(p1,3); 					//�õ��ɼ���������
    if(posx!=0XFF)gpsx->svnum=NMEA_Str2num(p1+posx,&dx);
    for(i=0;i<len;i++)
    {
        p1=(u8*)strstr((const char *)p,"$GNGSV");
        for(j=0;j<4;j++)
        {
            posx=NMEA_Comma_Pos(p1,4+j*4);
            if(posx!=0XFF)gpsx->slmsg[slx].num=NMEA_Str2num(p1+posx,&dx);	//�õ����Ǳ��
            else break;
            posx=NMEA_Comma_Pos(p1,5+j*4);
            if(posx!=0XFF)gpsx->slmsg[slx].eledeg=NMEA_Str2num(p1+posx,&dx);//�õ���������
            else break;
            posx=NMEA_Comma_Pos(p1,6+j*4);
            if(posx!=0XFF)gpsx->slmsg[slx].azideg=NMEA_Str2num(p1+posx,&dx);//�õ����Ƿ�λ��
            else break;
            posx=NMEA_Comma_Pos(p1,7+j*4);
            if(posx!=0XFF)gpsx->slmsg[slx].sn=NMEA_Str2num(p1+posx,&dx);	//�õ����������
            else break;
            slx++;
        }
        p=p1+1;//�л�����һ��GPGSV��Ϣ
    }
}
//����GPGGA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,u8 *buf)
{
    u8 *p1,dx;
    u8 posx;
    p1=(u8*)strstr((const char *)buf,"$GNGGA");
    posx=NMEA_Comma_Pos(p1,6);								//�õ�GPS״̬
    printk("posx = %d\r\n", posx);
    if(posx!=0XFF)gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);
    printk("gpssta = %d\r\n", gpsx->gpssta);
    posx=NMEA_Comma_Pos(p1,7);								//�õ����ڶ�λ��������
    printk("posx = %d\r\n", posx);
    if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx);
    posx=NMEA_Comma_Pos(p1,9);								//�õ����θ߶�
    if(posx!=0XFF)gpsx->altitude=NMEA_Str2num(p1+posx,&dx);
}
//����GPGSA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,u8 *buf)
{
    u8 *p1,dx;
    u8 posx;
    u8 i;
    p1=(u8*)strstr((const char *)buf,"$GNGSA");
    posx=NMEA_Comma_Pos(p1,2);								//�õ���λ����
    if(posx!=0XFF)gpsx->fixmode=NMEA_Str2num(p1+posx,&dx);
    for(i=0;i<12;i++)										//�õ���λ���Ǳ��
    {
        posx=NMEA_Comma_Pos(p1,3+i);
        if(posx!=0XFF)gpsx->possl[i]=NMEA_Str2num(p1+posx,&dx);
        else break;
    }
    posx=NMEA_Comma_Pos(p1,15);								//�õ�PDOPλ�þ�������
    if(posx!=0XFF)gpsx->pdop=NMEA_Str2num(p1+posx,&dx);
    posx=NMEA_Comma_Pos(p1,16);								//�õ�HDOPλ�þ�������
    if(posx!=0XFF)gpsx->hdop=NMEA_Str2num(p1+posx,&dx);
    posx=NMEA_Comma_Pos(p1,17);								//�õ�VDOPλ�þ�������
    if(posx!=0XFF)gpsx->vdop=NMEA_Str2num(p1+posx,&dx);
}
//����GPRMC��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
    u8 *p1,dx;
    u8 posx;
    u32 temp;
    float rs;
    p1=(u8*)strstr((const char *)buf,"GNRMC");//"$GPRMC",������&��GPRMC�ֿ������,��ֻ�ж�GPRMC.
    posx=NMEA_Comma_Pos(p1,1);								//�õ�UTCʱ��
    if(posx!=0XFF)
    {
        temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//�õ�UTCʱ��,ȥ��ms
        gpsx->utc.hour=temp/10000;
        gpsx->utc.min=(temp/100)%100;
        gpsx->utc.sec=temp%100;
    }
    posx=NMEA_Comma_Pos(p1,3);								//�õ�γ��
    if(posx!=0XFF)
    {
        temp=NMEA_Str2num(p1+posx,&dx);
        gpsx->latitude=temp/NMEA_Pow(10,dx+2);	//�õ���
        rs=temp%NMEA_Pow(10,dx+2);				//�õ�'
        gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//ת��Ϊ��
    }
    posx=NMEA_Comma_Pos(p1,4);								//��γ���Ǳ�γ
    if(posx!=0XFF)gpsx->nshemi=*(p1+posx);
    posx=NMEA_Comma_Pos(p1,5);								//�õ�����
    if(posx!=0XFF)
    {
        temp=NMEA_Str2num(p1+posx,&dx);
        gpsx->longitude=temp/NMEA_Pow(10,dx+2);	//�õ���
        rs=temp%NMEA_Pow(10,dx+2);				//�õ�'
        gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//ת��Ϊ��
    }
    posx=NMEA_Comma_Pos(p1,6);								//������������
    if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);
    posx=NMEA_Comma_Pos(p1,9);								//�õ�UTC����
    if(posx!=0XFF)
    {
        temp=NMEA_Str2num(p1+posx,&dx);		 				//�õ�UTC����
        gpsx->utc.date=temp/10000;
        gpsx->utc.month=(temp/100)%100;
        gpsx->utc.year=2000+temp%100;
    }
}
//����GPVTG��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,u8 *buf)
{
    u8 *p1,dx;
    u8 posx;
    p1=(u8*)strstr((const char *)buf,"$GNVTG");
    posx=NMEA_Comma_Pos(p1,7);								//�õ���������
    if(posx!=0XFF)
    {
        gpsx->speed=NMEA_Str2num(p1+posx,&dx);
        if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);	 	 		//ȷ������1000��
    }
}
//��ȡNMEA-0183��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void GPS_Analysis(nmea_msg *gpsx,u8 *buf)
{
    NMEA_GPGSV_Analysis(gpsx,buf);	//GPGSV����
    NMEA_GPGGA_Analysis(gpsx,buf);	//GPGGA����
    NMEA_GPGSA_Analysis(gpsx,buf);	//GPGSA����
    NMEA_GPRMC_Analysis(gpsx,buf);	//GPRMC����
    NMEA_GPVTG_Analysis(gpsx,buf);	//GPVTG����
}
