#ifndef __GE_COMMON_H__
#define __GE_COMMON_H__
#include "data_type.h"

#define ENGINE_INTERVAL     30
#define AT_INTERVAL         40
#define ABS_INTERVAL        50
#define BCM_INTERVAL        60
#define IPC_INTERVAL        55
#define ERROR_CODE_INTERVAL 0

DataType vehicle_data[] =
{
	/*eng_data_rpm*/
	{
		ENG_DATA_RPM,
		/*name*/
		"eng_data_rpm",
        /* tx id */
        0x7e0,
        /* rx id */
        0x7e8,
		/*offset*/
        0x0a,
		/*size*/
        2,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
        ENGINE_INTERVAL,
	},
	/*eng_data_vs*/
	{
		ENG_DATA_VS,
		/*name*/
		"eng_data_vs",
		/* tx id */
        0x7e0,
        /* rx id */
        0x7e8,
		/*offset*/
        0x0c,
		/*size*/
        1,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_etc*/
	{
		ENG_DATA_ECT,
		/*name*/
		"eng_data_ect",
		/* tx id */
        0x7e0,
        /* rx id */
        0x7e8,
		/*offset*/
        0x09,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_iat*/
	{
		ENG_DATA_IAT,
		/*name*/
		"eng_data_iat",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x06,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_app*/
	{
		ENG_DATA_APP,
		/*name*/
		"eng_data_app",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x05,
		/*size*/
		2,
		/*tx buffer*/
		{0x02, 0x21, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_tp*/
	{
		ENG_DATA_TP,
		/*name*/
		"eng_data_tp",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x10,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_ert*/
	{
		ENG_DATA_ERT,
		/*name*/
		"eng_data_ert",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x0d,
		/*size*/
		2,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_load*/
	{
		ENG_DATA_LOAD,
		/*name*/
		"eng_data_load",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x01,
		/*size*/
		2,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_ltft*/
	{
		ENG_DATA_LTFT,
		/*name*/
		"eng_data_ltft",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x03,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_stft*/
	{
		ENG_DATA_STFT,
		/*name*/
		"eng_data_stft",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x02,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_misfire1*/
	{
		ENG_DATA_MISFIRE1,
		/*name*/
		"eng_data_misfire1",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x05,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_misfire2*/
	{
		ENG_DATA_MISFIRE2,
		/*name*/
		"eng_data_misfire2",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x06,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_misfire3*/
	{
		ENG_DATA_MISFIRE3,
		/*name*/
		"eng_data_misfire3",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x07,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_misfire4*/
	{
		ENG_DATA_MISFIRE4,
		/*name*/
		"eng_data_misfire4",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x08,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
#if 0
	/*eng_data_misfire5*/
	{
		ENG_DATA_MISFIRE5,
		/*name*/
		"eng_data_misfire5",

	},
	/*eng_data_misfire6*/
	{
		ENG_DATA_MISFIRE6,
		/*name*/
		"eng_data_misfire6",

	},
#endif
	/*eng_data_FCLS*/
	{
		ENG_DATA_FCLS,
		/*name*/
		"eng_data_FCLS",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x00,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
#if 0
	/*eng_data_keystatus*/
	{
		ENG_DATA_KEYSTATUS,
		/*name*/
		"eng_data_keystatus",

	},
#endif
	/*eng_data_ho2s1*/
	{
		ENG_DATA_HO2S1,
		/*name*/
		"eng_data_ho2s1",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x14,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_ho2s2*/
	{
		ENG_DATA_HO2S2,
		/*name*/
		"eng_data_ho2s2",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x18,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_map*/
	{
		ENG_DATA_MAP,
		/*name*/
		"eng_data_map",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x05,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
#if 0
	/*eng_data_injectpulse*/
	{
		ENG_DATA_INJECTPULSE,
		/*name*/
		"eng_data_injectpulse",
	},
#endif
	/*eng_data_oilpressure*/
	{
		ENG_DATA_OILPRESSURE,
		/*name*/
		"eng_data_oilpressure",
		/* tx id */
        0x7c0,
        /* rx id */
		0x7c8,
		/*offset*/
        0x04,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		IPC_INTERVAL,
	},

	/*eng_data_oillevelstatus*/
	{
		ENG_DATA_OILLEVELSTATUS,
		/*name*/
		"eng_data_oillevelstatus",
		/* tx id */
        0x7c0,
        /* rx id */
		0x7c8,
		/*offset*/
        0x01,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		IPC_INTERVAL,
	},

	/*eng_data_af*/
	{
		ENG_DATA_AF,
		/*name*/
		"eng_data_af",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x02,
		/*size*/
		2,
		/*tx buffer*/
		{0x02, 0x21, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_igtiming*/
	{
		ENG_DATA_IGTIMING,
		/*name*/
		"eng_data_igtiming",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x08,
		/*size*/
		2,
		/*tx buffer*/
		{0x02, 0x21, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
	/*eng_data_MAF*/
	{
		ENG_DATA_MAF,
		/*name*/
		"eng_data_maf",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x03,
		/*size*/
		2,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
#if 0
	/*eng_data_oillife*/
	{
		ENG_DATA_OILLIFE,
		/*name*/
		"eng_data_oillife",
	},
#endif
	/*eng_data_oiltemp*/
	{
		ENG_DATA_OILTEMP,
		/*name*/
		"eng_data_oiltemp",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x31,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},
#if 0
	/*eng_data_fuel*/
	{
		ENG_DATA_FUEL,
		/*name*/
		"eng_data_fuel",
	},
#endif
		/*eng_data_fuellevel*/
	{
		ENG_DATA_FUELLEVEL,
		/*name*/
		"eng_data_fuellevel",
		/* tx id */
        0x7c0,
        /* rx id */
		0x7c8,
		/*offset*/
        0x01,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		IPC_INTERVAL,
	},
#if 0
		/*eng_data_fueltank*/
	{
		ENG_DATA_FUELTANK,
		/*name*/
		"eng_data_fueltank",
	},

#endif
		/*at_data_oiltemp*/
	{
		AT_DATA_OILTEMP,
		/*name*/
		"at_data_oiltemp",
		/* tx id */
        0x7B0,
        /* rx id */
		0x7B8,
		/*offset*/
        0x02,
		/*size*/
		2,
		/*tx buffer*/
		{0x02, 0x21, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		IPC_INTERVAL,
	},

#if 0
		/*abs_data_oillevel*/
	{
		ABS_DATA_OILLEVEL,
		/*name*/
		"abs_data_oillevel",
		/*txrx id*/
		{0x243, 0x643, 0x243, 0x543},
	},
		/*bcm_data_chargestatus*/  
	{
		BCM_DATA_CHARGESTATUS,
		/*name*/
		"bcm_data_chargestatus",
	},
	/*bcm_data_battcurrent*/  
	{
		BCM_DATA_BATTCURRENT,
		/*name*/
		"bcm_data_battcurrent",
	},
		/*bcm_data_battstatus*/  
	{
		BCM_DATA_BATTSTATUS,
		/*name*/
		"bcm_data_battstatus",
		/*txrx id*/
		{0x241, 0x641, 0x241, 0x641},
	},
#endif
		/*bcm_data_battvolt*/  
	{
		BCM_DATA_BATTVOLT,
		/*name*/
		"bcm_data_battvolt",
		/* tx id */
        0x7e0,
        /* rx id */
		0x7e8,
		/*offset*/
        0x1a,
		/*size*/
		2,
		/*tx buffer*/
		{0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		BCM_INTERVAL,
	},
	/*bcm_data_dda*/  
	{
		BCM_DATA_DDA,
		/*name*/
		"bcm_data_dda",
		/* tx id */
        0x750,
        /* rx id */
		0x758,
		/*offset*/
        0x02,
		/*size*/
		1,
		/*tx buffer*/
		{0x40, 0x02, 0x21, 0xA2, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		BCM_INTERVAL,
	},

	/*bcm_data_pda*/  
	{
		BCM_DATA_PDA,
		/*name*/
		"bcm_data_pda",
		/* tx id */
        0x750,
        /* rx id */
		0x758,
		/*offset*/
        0x02,
		/*size*/
		1,
		/*tx buffer*/
		{0x40, 0x02, 0x21, 0xA3, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		BCM_INTERVAL,
	},
	/*bcm_data_rrda*/  
	{
		BCM_DATA_RRDA,
		/*name*/
		"bcm_data_rrda",
		/* tx id */
        0x750,
        /* rx id */
		0x758,
		/*offset*/
        0x04,
		/*size*/
		1,
		/*tx buffer*/
		{0x40, 0x02, 0x21, 0xA3, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		BCM_INTERVAL,
	},
		/*bcm_data_lrda*/  
		{
		BCM_DATA_LRDA,
		/*name*/
		"bcm_data_lrda",
		/* tx id */
        0x750,
        /* rx id */
		0x758,
		/*offset*/
        0x05,
		/*size*/
		1,
		/*tx buffer*/
		{0x40, 0x02, 0x21, 0xA3, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		BCM_INTERVAL,
	},
#if 0
	/*bcm_data_sunroof*/  
	{
		BCM_DATA_SUNROOF,
		/*name*/
		"bcm_data_sunroof",
		/* tx id */
        0x750,
        /* rx id */
		0x758,
		/*offset*/
        0x03,
		/*size*/
		1,
		/*tx buffer*/
		{0xAD,0x02, 0x21, 0xE6, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		BCM_INTERVAL,
	},
	/*bcm_data_headlamp*/  
	{
		BCM_DATA_HEADLAMP,
		/*name*/
		"bcm_data_headlamp",
		/* tx id */
        0x750,
        /* rx id */
		0x758,
		/*offset*/
        0x03,
		/*size*/
		1,
		/*tx buffer*/
		{0x70, 0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		BCM_INTERVAL,
	},
	/*bcm_data_highbeam*/  
	{
		BCM_DATA_HIGHBEAM,
		/*name*/
		"bcm_data_highbeam",
		/* tx id */
        0x750,
        /* rx id */
		0x758,
		/*offset*/
        0x03,
		/*size*/
		1,
		/*tx buffer*/
		{0x70, 0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		BCM_INTERVAL,
	},
#endif
	/*bcm_data_hazard*/  
{
		BCM_DATA_HAZARD,
		/*name*/
		"bcm_data_hazard",
		/* tx id */
        0x7C0,
        /* rx id */
		0x7C8,
		/*offset*/
        0x05,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		IPC_INTERVAL,
	},
	/*bcm_data_frontfog*/  
    {
		BCM_DATA_FRONTFOG,
		/*name*/
		"bcm_data_frontfog",
		/* tx id */
        0x750,
        /* rx id */
		0x758,
		/*offset*/
        0x02,
		/*size*/
		1,
		/*tx buffer*/
		{0x40, 0x02, 0x21, 0xA7, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		BCM_INTERVAL,
	},
		/*bcm_data_rearfog*/  
	{
		BCM_DATA_REARFOG,
		/*name*/
		"bcm_data_rearfog",
		/* tx id */
        0x7c0,
        /* rx id */
		0x7c8,
		/*offset*/
        0x02,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0xB1, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		IPC_INTERVAL,
	},
	/*bcm_data_leftturn*/  
	{
		BCM_DATA_LEFTTURN,
		/*name*/
		"bcm_data_leftturn",
		/* tx id */
        0x7c0,
        /* rx id */
		0x7c8,
		/*offset*/
        0x04,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		IPC_INTERVAL,
	},
		/*bcm_data_rightturn*/  
	{
		BCM_DATA_RIGHTTURN,
		/*name*/
		"bcm_data_rightturn",
		/* tx id */
        0x7c0,
        /* rx id */
		0x7c8,
		/*offset*/
        0x04,
		/*size*/
		1,
		/*tx buffer*/
		{0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		IPC_INTERVAL,
	},
		/*bcm_data_odo*/
     {
		BCM_DATA_ODO,
		/*name*/
		"bcm_data_odo",
		/* tx id */
        0x7E0,
        /* rx id */
		0x7E8,
		/*offset*/
        0x02,
		/*size*/
		3,
		/*tx buffer*/
		{0x02, 0x21, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00},
		/*interval*/
		ENGINE_INTERVAL,
	},

};

#endif
