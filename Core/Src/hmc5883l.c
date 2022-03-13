/*
 * hmc5883l.c
 *
 *  Created on: Mar 9, 2022
 *      Author: root
 */

#include "main.h"
#include "hmc5883l.h"


uint8_t Read8(HMC5883L_t *hmc , uint8_t Register)
{
	uint8_t Value;

	HAL_I2C_Mem_Read(hmc->hmc5883l_i2c, (hmc->Address)<<1, Register, 1, &Value, 1,
			HMC5883L_i2c_timeout);
	return Value;

}
uint8_t Read16(HMC5883L_t *hmc , uint8_t Register){
	uint8_t Value[2];
	HAL_I2C_Mem_Read(hmc->hmc5883l_i2c, (hmc->Address)<<1, Register, 1, Value, 2,
				HMC5883L_i2c_timeout);
	return ((Value[1]<<8) | Value[0]);
}
void Write8(HMC5883L_t *hmc , uint8_t Register , uint8_t Value)
{

	HAL_I2C_Mem_Write(hmc->hmc5883l_i2c, (hmc->Address)<<1, Register, 1, &Value, 1,HMC5883L_i2c_timeout);
}
// Configuration register A for HMC5883L

void HMC5883L_MeasureMode(HMC5883L_t *hmc , uint8_t Mode)
{
	uint8_t Tmp;
	if(Mode>3) Mode =3;

	Tmp = Read8(hmc , HMC5883L_RA_CONFIG_A );
	Tmp = Tmp |(Mode  << 0)	;
	//Tmp = Tmp & 0xFC;
	//Tmp |= Mode ;

	Write8(hmc , HMC5883L_RA_CONFIG_A  ,Tmp);
}
void HMC5883L_DataOutputRate(HMC5883L_t *hmc , uint8_t Mode){
	uint8_t Tmp;
	Tmp = Read8(hmc , HMC5883L_RA_CONFIG_A );
	Tmp = Tmp |(Mode << 2 )	;			// a = a | (1 << 3)

	Write8(hmc , HMC5883L_RA_CONFIG_A  ,Tmp);

}

void HMC5883L_SamplesAveraged(HMC5883L_t *hmc , uint8_t Mode){
	uint8_t Tmp;
	Tmp = Read8(hmc , HMC5883L_RA_CONFIG_A );
	Tmp = Tmp |(Mode << 5 )	;			// a = a | (1 << 3)

	Write8(hmc , HMC5883L_RA_CONFIG_A  ,Tmp);

}
// Configuration register B for HMC5883L
void HMC5883L_Gain(HMC5883L_t *hmc , uint8_t Mode){
	uint8_t Tmp;
	Tmp = Read8(hmc , HMC5883L_RA_CONFIG_B );
	Tmp = Tmp |(Mode << 5 )	;			// a = a | (1 << 3)

	Write8(hmc , HMC5883L_RA_CONFIG_B  ,Tmp);

}
//Configuration Mode Register
//Operating Mode
/*
 * 		HMC5883L_MODE_CONTINUOUS
 	 	HMC5883L_MODE_SINGLE
 	 	HMC5883L_MODE_IDLE
 */
void HMC5883L_OperatingMode(HMC5883L_t *hmc , uint8_t Mode){
	uint8_t Tmp;
	Tmp = Read8(hmc , HMC5883L_RA_MODE  );
	Tmp = Tmp |(Mode << 0 )	;			// a = a | (1 << 3)

	Write8(hmc , HMC5883L_RA_MODE   ,Tmp);

}


/** Power on and prepare for general usage.
 * This will prepare the magnetometer with default settings, ready for single-
 * use mode (very low power requirements). Default settings include 8-sample
 * averaging, 15 Hz data output rate, normal measurement bias, a,d 1090 gain (in
 * terms of LSB/Gauss). Be sure to adjust any settings you need specifically
 * after initialization, especially the gain settings if you happen to be seeing
 * a lot of -4096 values (see the datasheet for mor information).
 */

uint16_t osx(HMC5883L_t *hmc, I2C_HandleTypeDef *i2c, uint8_t Address  ){
	uint16_t X;
	uint16_t Z;
	uint16_t Y;

	X = Read16(hmc, HMC5883L_RA_DATAX_H );
	Z = Read16(hmc, HMC5883L_RA_DATAZ_H );
	Y = Read16(hmc, HMC5883L_RA_DATAY_H  );
	if (X > 5000){
		return 1;
	}
	if (Y > 5000){
			return 1;
		}
	if (Z > 5000){
			return 1;
		}
	return 1;
}


uint8_t   HMC5883L_Init(HMC5883L_t *hmc, I2C_HandleTypeDef *i2c, uint8_t Address  ) {

	uint8_t RA;
	uint8_t RB;
	uint8_t RC;
	uint8_t RD;

	hmc-> hmc5883l_i2c = i2c;
	hmc-> Address = Address;
	HMC5883L_OperatingMode(hmc , HMC5883L_MODE_CONTINUOUS);
	RA = Read8(hmc,HMC5883L_RA_CONFIG_A   );
	RB = Read8(hmc,HMC5883L_RA_ID_B   );
	RC = Read8(hmc,HMC5883L_RA_ID_C    );
	RD = Read8(hmc,HMC5883L_RA_MODE   );

	if (RA  == 15)  //ASCII "H"
	{
		return 1;

	}
	if (RB == 1){
		return 1;
	}
	if (RC  == 1){
		return 1;
	}
	if (RD == 1){
		return 1;
	}

	HMC5883L_DataOutputRate(hmc,1);
 	HMC5883L_MeasureMode(hmc , 1);

 	RD =Read8(hmc,HMC5883L_RA_CONFIG_A   );
 	if (RD == 0x20)
 	{
 		return 1;

 	}
 	return 0;
    // write CONFIG_A register
    //HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout)
   // I2Cdev_writeByte(devAddr, HMC5883L_RA_CONFIG_A,
    		//(     << (       ))
    		//(0x03 << (6-2 +1 ))
        //(HMC5883L_AVERAGING_8 << (HMC5883L_CRA_AVERAGE_BIT - HMC5883L_CRA_AVERAGE_LENGTH + 1)) |
       // (HMC5883L_RATE_15     << (HMC5883L_CRA_RATE_BIT - HMC5883L_CRA_RATE_LENGTH + 1)) |
       // (HMC5883L_BIAS_NORMAL << (HMC5883L_CRA_BIAS_BIT - HMC5883L_CRA_BIAS_LENGTH + 1)));

    // write CONFIG_B register
    //HMC5883L_setGain(HMC5883L_GAIN_1090);

    // write MODE register
    //HMC5883L_setMode(HMC5883L_MODE_SINGLE);
}





