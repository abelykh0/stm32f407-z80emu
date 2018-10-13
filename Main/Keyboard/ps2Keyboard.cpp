#include "stm32f4xx.h"
#include "ps2keyboard.h"
#include "queue.h"

typedef enum
{
    IDLE = 0,
    BIT0,
    BIT1,
    BIT2,
    BIT3,
    BIT4,
    BIT5,
    BIT6,
    BIT7,
    PARITY,
    STOP
} ps2_read_status;

ps2_read_status ps2_status;
int32_t kb_data;

uint8_t lastClk = 1;
uint8_t lastData;
volatile uint8_t _parity;

void Ps2_Initialize()
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    // CLK pin
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // DATA pin
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    ps2_status = IDLE;
    QueueInit();
    kb_data = 0;
}

int32_t Ps2_GetScancode()
{
    int32_t result;
    if (QueueGet(&result) == -1)
    {
        result = -1;
    }

    return result;
}

inline void Update(uint8_t dataBit)
{
    switch (ps2_status)
    {
    case IDLE:
        ps2_status = BIT0;
        _parity = 0;
        break;
    case BIT0:
        ps2_status = BIT1;
        kb_data |= dataBit;
        _parity += dataBit;
        break;
    case BIT1:
        ps2_status = BIT2;
        kb_data = kb_data | (dataBit << 1);
        _parity += dataBit;
        break;
    case BIT2:
        ps2_status = BIT3;
        kb_data = kb_data | (dataBit << 2);
        _parity += dataBit;
        break;
    case BIT3:
        ps2_status = BIT4;
        kb_data = kb_data | (dataBit << 3);
        _parity += dataBit;
        break;
    case BIT4:
        ps2_status = BIT5;
        kb_data = kb_data | (dataBit << 4);
        _parity += dataBit;
        break;
    case BIT5:
        ps2_status = BIT6;
        kb_data = kb_data | (dataBit << 5);
        _parity += dataBit;
        break;
    case BIT6:
        ps2_status = BIT7;
        kb_data = kb_data | (dataBit << 6);
        _parity += dataBit;
        break;
    case BIT7:
        ps2_status = PARITY;
        kb_data = kb_data | (dataBit << 7);
        _parity += dataBit;
        break;
    case PARITY:
        _parity &= 1;
        if (_parity == dataBit) 
        {
            // Parity error
            _parity = 0xFD;
            kb_data = 0;

            // TODO ask to resend
        }
        ps2_status = STOP;
        break;
    case STOP:
        ps2_status = IDLE;
        if (_parity != 0xFD)
        {
            if ((kb_data & 0xE0) == 0xE0)
            {
                kb_data <<= 8;
            }
            else
            {
                QueuePut(kb_data);
                kb_data = 0;
            }
        }
        break;
    }
}

__attribute__((section(".ramcode")))
void vga_hblank_interrupt()
{
    uint32_t gpioBits = GPIOB->IDR;
    uint8_t clkBit = (gpioBits & 0x4000) ? 1 : 0;
    uint8_t dataBit = (gpioBits & 0x2000) ? 1 : 0;

    if (clkBit == 0)
    {
        // CLK = 0

        lastData = dataBit;
        lastClk = 0;
    }
    else
    {
        // CLK = 1

        if (lastClk == 0)
        {
            Update(lastData);
            lastClk = 1;
        }
        else
        {
            if (ps2_status != IDLE)
            {
                lastClk++;
                if (lastClk > 10)
                {
                    // Timeout
                    ps2_status = IDLE;
                    kb_data = 0;

                    // TODO ask to resend
                }
            }
            else
            {
                lastClk = 1;
            }
        }
    }
}
