#include "main.h"
#include "I2C.h"
#include "MCP3428.h"

struct MCP3428 ExtAdc;

/**
 * \fn void MCP3428_Init()
 * \brief Initialise l'ADC MCP3428 avec les paramètres par défaut.
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
 * \brief Lit tous les canaux ADC et convertit les valeurs brutes en tensions (mV).
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
 * \brief Convertit une valeur brute en tension (mV) selon le gain appliqué.
 * selon le gain que l'on applique au cannal
 * @param RawVal Valeur brute ADC.
 * @param gain Gain appliqué (G1, G2, G4, G8).
 * \return Tension convertie en millivolts.
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
 * \brief Lit les valeurs brutes de tous les canaux ADC.
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
 * \brief Lit la donnée d'un canal donné et la stocke dans la structure.
 * @param ch Canal à lire (0 à 3).
 * @param gain Gain à appliquer.
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
 * \brief Sélectionne un canal et un gain, met à jour la config du MCP3428.
 * @param ch Canal sélectionné.
 * @param gain Gain appliqué.
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
 * \brief Génère l'octet de configuration selon les paramètres de la structure.
 * @return Byte de configuration.
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
 * @brief Écrit un registre sur le MCP3428 via I2C.
 * @param SlaveAdr Adresse I2C du MCP3428.
 * @param RegVal Valeur du registre à écrire.
 * \return void
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
 * @brief Lit une valeur 16 bits depuis le MCP3428 via I2C.
 * @param SlaveAdr Adresse I2C du MCP3428.
 * @return Valeur ADC lue (16 bits).
 */
uint16_t MCP3428_R_DATA(uint8_t SlaveAdr)
{
    uint8_t val[2] = {0};
    uint16_t ReturnVal = 0;
    
    I2C_Start();
    
    I2C_Write((SlaveAdr << 1));
    
    I2C_Restart();
    I2C_Write(((SlaveAdr << 1)|0x01));


    val[0] = I2C_Read();
    I2C_ACK();
    
    val[1] = I2C_Read();
    I2C_NACK();
    
    I2C_Stop();
    
    ReturnVal = (val[0] << 8) + val[1];
            
    return(ReturnVal);
}