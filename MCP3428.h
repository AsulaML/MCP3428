#define MCP3428_SLAVE_ADDR 0b1101000

#define MCP3428_240_SPS 0b00
#define MCP3428_60_SPS 0b01
#define MCP3428_15_SPS 0b10

#define MCP3428_G1 1
#define MCP3428_G2 2
#define MCP3428_G4 4
#define MCP3428_G8 5

#define MCP3428_GX1 0b00
#define MCP3428_GX2 0b01
#define MCP3428_GX4 0b10
#define MCP3428_GX8 0b11

#define MCP3428_CONTINUOUS 0b1
#define MCP3428_ONE_SHOT 0b0

#define MCP3428_CH1 0b00
#define MCP3428_CH2 0b01
#define MCP3428_CH3 0b10
#define MCP3428_CH4 0b11

#define MCP3428_RATIO_MV (MCP3428_VREF_MV/MCP3428_RES)
#define MCP3428_VREF_MV 2048.0
#define MCP3428_RES 32767.5

struct MCP3428
{
    uint8_t Gain;
    uint8_t SampleRate;
    uint8_t ConversionMode;
    uint8_t ChannelSelected;
    
    int16_t ChannelRawVal[4];
    float ChannelVoltage_mV[4];
};


void MCP3428_Init();
void MCP3428_Read_All_CH_ConvertToVoltage();
float MCP3428_ConvertRawToVoltage(int16_t RawVal, uint8_t gain);
void MCP3428_Read_All_CH();
void MCP3428_Read_CH(uint8_t ch, uint8_t gain);
void MCP3428_Select_Channel_With_Gain(uint8_t ch, uint8_t gain);
uint8_t MCP3428_ConfigByte_Format();

void MCP3428_W_REG(uint8_t SlaveAdr, uint8_t RegVal);
uint16_t MCP3428_R_DATA(uint8_t SlaveAdr);