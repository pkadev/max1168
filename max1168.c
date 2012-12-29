#include <avr/io.h>
#include "max1168.h"

/* Hardware dependent defines */
#define MAX1168_CS_DDR          DDRD
#define MOSI                    PB3
#define SCK                     PB5
#define MAX1168_CS_PIN          PD7
#define DDR_SPI                 DDRB 
#define MAX1168_CS_PORT         PORTD
#define EOC                     PB0
#define DSEL                    PB2
/* End hardware dependent defines */

#define CS_LOW()    (MAX1168_CS_PORT &= ~(1<<MAX1168_CS_PIN))
#define CS_HIGH()   (MAX1168_CS_PORT |= (1<<MAX1168_CS_PIN))
#define SPI_WAIT()  while(!(SPSR & (1<<SPIF)))

static void max1168_init(void)
{
    /* Set MOSI and SCK output */
    DDR_SPI |= (1<<MOSI) | (1<<SCK);
    DDR_SPI |= (1<<DSEL);

    /* CS is output */
    MAX1168_CS_DDR |= (1<<MAX1168_CS_PIN);
    /* SPI SS pin as output to aviod slave mode */
    //SPSR |= (1<<SPI2X);
    /*
     * Enable SPI, Master, set clock rate.
     * max1168 can run in 4.8MHz max
     */
    SPCR = (1<<SPE) | (1<<MSTR);
    SPCR |= (1<<SPR0);
    CS_HIGH();
}

static uint8_t max1168_xfer_byte(uint8_t tx)
{
    SPDR = tx;
    SPI_WAIT();
    return SPDR;
}

uint16_t max1168_mean(enum max1168_channel ch, uint8_t samples)
{
    uint32_t tmp = 0;
    uint8_t i;
#define MAX_ALLOWED_SAMPLES 10
    if (samples > MAX_ALLOWED_SAMPLES)
        return 0;

    for (i = 0; i < samples; i++) {
       tmp += max1168_read_adc(SPI_DUMMY_BYTE, CLK_EXTERNAL, MODE_8BIT); 
    }
    tmp /= samples;
    return (uint16_t)tmp;   
}

uint16_t max1168_read_adc(uint8_t reg, enum max1168_clk clk, enum max1168_mode mode)
{
    #define CH2 0x20
    uint16_t raw_data;
    max1168_init();

    if (mode == MODE_8BIT)
        PORTB &= ~(1<<DSEL);
    else
        PORTB |= (1<<DSEL);

    CS_LOW();
    /* TODO: remove hardcoded channel */
    max1168_xfer_byte(CH2 | clk);

    if (mode == MODE_16BIT)
        max1168_xfer_byte(SPI_DUMMY_BYTE);

    if (clk == CLK_INTERNAL)
        while ((PINB & (1<<EOC)));

    raw_data = max1168_xfer_byte(SPI_DUMMY_BYTE) << 8;
    raw_data |= max1168_xfer_byte(SPI_DUMMY_BYTE);
    CS_HIGH();

    return raw_data;
}
