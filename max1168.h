#ifndef MAX1168_H_
#define MAX1168_H_

#include <stdint.h>

#define SPI_DUMMY_BYTE          0xff

enum max1168_channel {
    CH0,
    CH1
};
enum max1168_clk {
    CLK_EXTERNAL,
    CLK_INTERNAL
};

enum max1168_mode {
    MODE_16BIT,
    MODE_8BIT
};

uint16_t max1168_mean(enum max1168_channel ch, uint8_t samples);
uint16_t max1168_read_adc(uint8_t reg, enum max1168_clk clk, enum max1168_mode mode);

#endif
