#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "math.h"
#include "pico/binary_info.h"
#include <stdio.h>

#define MAX31855_FAULT_NONE (0x00)		 ///< Disable all fault checks
#define MAX31855_FAULT_OPEN (0x01)		 ///< Enable open circuit fault check
#define MAX31855_FAULT_SHORT_GND (0x02)	 ///< Enable short to GND fault check
#define MAX31855_FAULT_SHORT_VCC (0x04)	 ///< Enable short to VCC fault check
#define MAX31855_FAULT_ALL (0x07)		 ///< Enable all fault checks

#define MAX31855_FAULT_S_NONE "NO FAULT\r\n"
#define MAX31855_FAULT_S_OPEN "FAULT OPEN\r\n"
#define MAX31855_FAULT_S_SHORT_GND "FAULT SHORT GND\r\n"
#define MAX31855_FAULT_S_SHORT_VCC "FAULT SHORT VCC\r\n"
#define MAX31855_FAULT_S_UNKNOW "FAULT UNKNOW\r\n"


#define MAX31855_SIGN_EXTEND(value, bit) (((value) ^ (1u << ((bit) -1))) - (1u << ((bit) -1)))

/**************************************************************************************/
/*!
	@brief  Sensor driver for the Adafruit MAX31855 thermocouple breakout for pico-sdk.
*/
/**************************************************************************************/
struct pico_MAX31855
{
	bool	initialized;
	uint8_t faultMask;
	uint	cs_pin;
	uint	mosi_pin;
	uint	clk_pin;
};

bool	 pico_MAX31855_init(struct pico_MAX31855* thermocouple, uint _mosi, uint _clk, uint _cs);
bool	 pico_MAX31855_begin(struct pico_MAX31855* thermocouple);
double	 pico_MAX31855_readInternal(struct pico_MAX31855* thermocouple);
double	 pico_MAX31855_readCelsius(struct pico_MAX31855* thermocouple);
double	 pico_MAX31855_readFahrenheit(struct pico_MAX31855* thermocouple);
uint8_t	 pico_MAX31855_readError(struct pico_MAX31855* thermocouple);
void	 pico_MAX31855_setFaultChecks(struct pico_MAX31855* thermocouple, uint8_t faults);
uint32_t pico_MAX31855_spiread32(struct pico_MAX31855* thermocouple);
void	 pico_MAX31855_cs_select(struct pico_MAX31855* thermocouple);
void	 pico_MAX31855_cs_deselect(struct pico_MAX31855* thermocouple);
void	 pico_MAX31855_printError(struct pico_MAX31855* thermocouple);
