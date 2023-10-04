#include "spacelight.h"

#define DMX512_BUFFER_LEN 512

typedef enum
{
    DEFAULT_NULL_START_CODE = 0X0,
    TEXT_PACKET = 0X17,
    RDM_START_CODE = 0XCC,
    SYSTEM_INFORMATION_PACKET = 0XCF,
} Dmx512StartCode;

uint8_t dmx512_buffer[DMX512_BUFFER_LEN];

inline void dmx512_setch(uint16_t ch, uint8_t value)
{
    dmx512_buffer[ch] = value;
}

void dmx512_start()
{
    /* enable tx/rx dma */

    /* fire dmx512 tx every second (250kbit/s)*/
}
