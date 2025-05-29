#include "main.h"
#include "I2C.h"
#include "MCP3428.h"

struct MCP3428 ExtAdc;

/**
 * \fn void MCP3428_Init()
 * \brief fonction qui permet d'initialiser l'adc
 * \return 0
 */
void MCP3428_Init()
{
    ExtAdc.ChannelSelected = MCP3428_CH1;
    ExtAdc.ConversionMode = MCP3428_CONTINUOUS;
    ExtAdc.Gain = MCP3428_GX1;
    ExtAdc.SampleRate = MCP3428_15_SPS;
            
    MCP3428_W_REG(MCP3428_SLAVE_ADDR, MCP3428_ConfigByte_Format());
}

/**
 * \fn void MCP3428_Read_All_CH_ConvertToVoltage()
 * \brief fonction qui permet de lires toutes les tensions d'entrées
 * \return 0
 */
void MCP3428_Read_All_CH_ConvertToVoltage()
{
    MCP3428_Read_All_CH();
    
    ExtAdc.ChannelVoltage_mV[0] = MCP3428_ConvertRawToVoltage(ExtAdc.ChannelRawVal[0], MCP3428_G1);
    ExtAdc.ChannelVoltage_mV[1] = MCP3428_ConvertRawToVoltage(ExtAdc.ChannelRawVal[1], MCP3428_G1);
    ExtAdc.ChannelVoltage_mV[2] = MCP3428_ConvertRawToVoltage(ExtAdc.ChannelRawVal[2], MCP3428_G1);
    ExtAdc.ChannelVoltage_mV[3] = MCP3428_ConvertRawToVoltage(ExtAdc.ChannelRawVal[3], MCP3428_G1);
}

/**
 * \fn float MCP3428_ConvertRawToVoltage(int16_t RawVal, uint8_t gain)
 * \brief fonction qui permet de convertir un int16_t en valeur de tension
 * selon le gain que l'on applique au cannal
 * \return float
 */
float MCP3428_ConvertRawToVoltage(int16_t RawVal, uint8_t gain)
{
    float ValConverted = 0;
    
    ValConverted = ((float)(RawVal) * MCP3428_RATIO_MV);
    ValConverted /= gain;
            
    return(ValConverted);
}

/**
 * \fn void MCP3428_Read_All_CH()
 * \brief fonction qui permet toutes les channels et de la l'ajouter à la structure
 * \return 0
 */
void MCP3428_Read_All_CH()
{
    MCP3428_Read_CH(MCP3428_CH1, MCP3428_GX1);
    MCP3428_Read_CH(MCP3428_CH2, MCP3428_GX1);
    MCP3428_Read_CH(MCP3428_CH3, MCP3428_GX1);
    MCP3428_Read_CH(MCP3428_CH4, MCP3428_GX1);
}

/**
 * \fn void MCP3428_Read_CH(uint8_t ch)
 * \brief fonction qui permet le lire la donnée d'un channel et de la placer dans la structure
 * \return 0
 */
void MCP3428_Read_CH(uint8_t ch, uint8_t gain)
{
    MCP3428_Select_Channel_With_Gain(ch, gain);
    __delay_ms(75);
    ExtAdc.ChannelRawVal[ch] = MCP3428_R_DATA(MCP3428_SLAVE_ADDR);
}

/**
 * \fn void MCP3428_Select_Channel_With_Gain(uint8_t ch)
 * \brief fonction qui permet sélectionner le cannal de mesure
 * \return 0
 */
void MCP3428_Select_Channel_With_Gain(uint8_t ch, uint8_t gain)
{
    ExtAdc.ChannelSelected = ch;
    ExtAdc.Gain = gain;
    
    MCP3428_W_REG(MCP3428_SLAVE_ADDR, MCP3428_ConfigByte_Format());
}

/**
 * \fn uint8_t MCP3428_ConfigByte_Format()
 * \brief fonction qui permet formater le byte de config selon les valeurs de la 
 * structure
 * \return le config byte
 */
uint8_t MCP3428_ConfigByte_Format()
{
    uint8_t ConfigByte = 0;
        
    ConfigByte = ExtAdc.Gain;
    ConfigByte |= (ExtAdc.SampleRate << 2);
    ConfigByte |= (ExtAdc.ConversionMode << 4);
    ConfigByte |= (ExtAdc.ChannelSelected << 5);
    
    return(ConfigByte);
}

/**
 * \fn void MCP3428_W_REG(uint8_t SlaveAdr, uint8_t RegVal)
 * \brief fonction qui permet d'écrire dans le registre de config
 * \return 0
 */
void MCP3428_W_REG(uint8_t SlaveAdr, uint8_t RegVal)
{
    I2C_Start();
    
    I2C_Write((SlaveAdr << 1));
    I2C_Write(RegVal);
    
    I2C_Stop();
}

/**
 * \fn uint16_t MCP3428_R_DATA(uint8_t SlaveAdr)
 * \brief fonction qui permet de lire la valeur ADC 
 * \return un entier 16 bit
 */
uint16_t MCP3428_R_DATA(uint8_t SlaveAdr)
{
    uint8_t val[2] = {0};
    uint16_t ReturnVal = 0;
    
    I2C_Start();
    
    I2C_Write((SlaveAdr << 1));
    
    val[0] = I2C_Read();
    I2C_ACK();
    
    val[1] = I2C_Read();
    I2C_NACK();
    
    I2C_Stop();
    
    ReturnVal = (val[0] << 8) + val[1];
            
    return(ReturnVal);
}