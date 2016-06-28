
#ifdef GLOBALS 
#define EXT
#else
#define EXT extern 
#endif

 
EXT unsigned char TxBuffer1[400]; 
 
EXT unsigned char TxBuffer2[]; 
EXT unsigned char RxBuffer1[400];
EXT unsigned char RxBuffer2[];
EXT unsigned char TxCounter1;
EXT unsigned int TxCounter2;
EXT volatile unsigned int RxCounter1; 
EXT volatile unsigned int RxCounter2;

EXT volatile unsigned char rec_f,tx_flag;
EXT volatile unsigned long Rec_Len;	
EXT volatile unsigned  int  milsec1,milsec2,milsec3;
