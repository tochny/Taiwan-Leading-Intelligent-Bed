/************************************************************************/
/*																		*/
/*	emsk_adc.c		--		Definition for PMOD AD2 library 	   		*/
/*																		*/
/************************************************************************/
/*	Author:		Alex Chih												*/
/*	Copyright 2019, NTUST												*/
/************************************************************************/
/*  File Description:													*/
/*		This file defines functions for PMOD AD2						*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	06/06/2019(AlexC): created											*/
/*	10/06/2019(AlexC): modified											*/
/*																		*/
/************************************************************************/


/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */
#include "emsk_adc.h"


/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */
static DEV_IIC *emsk_adc_sensor;
static uint32_t adc_sensor_slvaddr;



int32_t emsk_adc_init(uint32_t slv_addr)
{
	int32_t ercd = E_OK;
	uint8_t config[2];

	emsk_adc_sensor = iic_get_dev(DW_IIC_1_ID);
	EMSK_ADC_CHECK_EXP_NORTN(emsk_adc_sensor!=NULL);

	ercd = emsk_adc_sensor->iic_open(IIC_SPEED_HIGH, DEV_MASTER_MODE, DEV_POLL_METHOD);
	if ((ercd == E_OK) || (ercd == E_OPNED)) {
		ercd = emsk_adc_sensor->iic_control(IIC_CMD_SET_TARADDR, &slv_addr);
		adc_sensor_slvaddr = slv_addr;
	}

	config[0] = EMSK_ADC_MSK_I2C_HS;
	config[1] = EMSK_ADC_MSK_CH0;
	emsk_adc_sensor -> iic_write(config, 2);

error_exit:
	return ercd;
}

int32_t emsk_adc_read(int32_t *val)
{
	int32_t ercd = E_OK;
	uint8_t data[1];
	int32_t temp = 0;

	//emsk_adc_sensor = iic_get_dev(DW_IIC_1_ID);

	EMSK_ADC_CHECK_EXP_NORTN(emsk_adc_sensor!=NULL);
	EMSK_ADC_CHECK_EXP_NORTN(val!=NULL);

	ercd = emsk_adc_sensor->iic_read(data,1);
	temp = (int32_t)data << 8;
	ercd = emsk_adc_sensor->iic_read(data,1);
	temp = temp + ((int32_t)data);
	*val = temp;

error_exit:
	return ercd;
}
