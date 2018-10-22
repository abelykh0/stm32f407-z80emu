#include <ctype.h>

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
bool _isLeftShiftPressed;
bool _isRightShiftPressed;
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
    _isLeftShiftPressed = false;
    _isRightShiftPressed = false;
}

int32_t Ps2_GetScancode()
{
    int32_t result;
    if (QueueGet(&result) == -1)
    {
        result = -1;
    }

    int32_t code = result & 0xFF;
    if (code == KEY_LEFTSHIFT)
    {
    	_isLeftShiftPressed = (result & 0xFF00) != 0xF000;
    }
    if (code == KEY_RIGHTSHIFT)
    {
    	_isRightShiftPressed = (result & 0xFF00) != 0xF000;
    }

    return result;
}

char Ps2_ConvertScancode(int32_t scanCode)
{
	char result;

	switch (scanCode)
	{
	case KEY_A:
		result = 'a';
		break;
	case KEY_B:
		result = 'b';
		break;
	case KEY_C:
		result = 'c';
		break;
	case KEY_D:
		result = 'd';
		break;
	case KEY_E:
		result = 'e';
		break;
	case KEY_F:
		result = 'f';
		break;
	case KEY_G:
		result = 'g';
		break;
	case KEY_H:
		result = 'h';
		break;
	case KEY_I:
		result = 'i';
		break;
	case KEY_J:
		result = 'j';
		break;
	case KEY_K:
		result = 'k';
		break;
	case KEY_L:
		result = 'l';
		break;
	case KEY_M:
		result = 'm';
		break;
	case KEY_N:
		result = 'n';
		break;
	case KEY_O:
		result = 'o';
		break;
	case KEY_P:
		result = 'p';
		break;
	case KEY_Q:
		result = 'q';
		break;
	case KEY_R:
		result = 'r';
		break;
	case KEY_S:
		result = 's';
		break;
	case KEY_T:
		result = 't';
		break;
	case KEY_U:
		result = 'u';
		break;
	case KEY_V:
		result = 'v';
		break;
	case KEY_W:
		result = 'w';
		break;
	case KEY_X:
		result = 'x';
		break;
	case KEY_Y:
		result = 'y';
		break;
	case KEY_Z:
		result = 'z';
		break;
	case KEY_0:
		result = '0';
		break;
	case KEY_1:
		result = '1';
		break;
	case KEY_2:
		result = '2';
		break;
	case KEY_3:
		result = '3';
		break;
	case KEY_4:
		result = '4';
		break;
	case KEY_5:
		result = '5';
		break;
	case KEY_6:
		result = '6';
		break;
	case KEY_7:
		result = '7';
		break;
	case KEY_8:
		result = '8';
		break;
	case KEY_9:
		result = '9';
		break;
	case KEY_BACKSPACE:
		result = '\b';
		break;
	case KEY_SPACEBAR:
		result = ' ';
		break;
	case KEY_COMMA:
		result = ',';
		break;
	case KEY_MINUS:
		result = '-';
		break;
	case KEY_DOT:
		result = '.';
		break;
	case KEY_DIV:
		result = '/';
		break;
	case KEY_SINGLE:
		result = '`';
		break;
	case KEY_APOS:
		result = '\'';
		break;
	case KEY_SEMI:
		result = ';';
		break;
	case KEY_BACK:
		result = '\\';
		break;
	case KEY_OPEN_SQ:
		result = '[';
		break;
	case KEY_CLOSE_SQ:
		result = ']';
		break;
	case KEY_EQUAL:
		result = '=';
		break;
	default:
		result = '\0';
		break;
	}

	if (_isLeftShiftPressed || _isRightShiftPressed)
	{
		switch (scanCode)
		{
		case KEY_0:
			result = ')';
			break;
		case KEY_1:
			result = '!';
			break;
		case KEY_2:
			result = '@';
			break;
		case KEY_3:
			result = '#';
			break;
		case KEY_4:
			result = '$';
			break;
		case KEY_5:
			result = '%';
			break;
		case KEY_6:
			result = '^';
			break;
		case KEY_7:
			result = '&';
			break;
		case KEY_8:
			result = '*';
			break;
		case KEY_9:
			result = '(';
			break;
		case KEY_COMMA:
			result = '<';
			break;
		case KEY_MINUS:
			result = '_';
			break;
		case KEY_DOT:
			result = '>';
			break;
		case KEY_DIV:
			result = '?';
			break;
		case KEY_SINGLE:
			result = '~';
			break;
		case KEY_APOS:
			result = '"';
			break;
		case KEY_SEMI:
			result = ':';
			break;
		case KEY_BACK:
			result = '|';
			break;
		case KEY_OPEN_SQ:
			result = '{';
			break;
		case KEY_CLOSE_SQ:
			result = '}';
			break;
		case KEY_EQUAL:
			result = '+';
			break;
		default:
			result = toupper(result);
			break;
		}
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
