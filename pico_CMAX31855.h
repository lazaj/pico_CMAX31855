#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "math.h"

#define MAX31855_FAULT_NONE (0x00)		 ///< Disable all fault checks
#define MAX31855_FAULT_OPEN (0x01)		 ///< Enable open circuit fault check
#define MAX31855_FAULT_SHORT_GND (0x02)	 ///< Enable short to GND fault check
#define MAX31855_FAULT_SHORT_VCC (0x04)	 ///< Enable short to VCC fault check
#define MAX31855_FAULT_ALL (0x07)		 ///< Enable all fault checks

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
	double	t_internal;
	double	t_celsius;
	double	t_fahrenheit;
	uint8_t error;
	// spi_inst_t* spi;
};

bool	 pico_MAX31855_init(struct pico_MAX31855* thermocouple, uint _cs);
bool	 pico_MAX31855_begin(struct pico_MAX31855* thermocouple);
bool	 pico_MAX31855_readInternal(struct pico_MAX31855* thermocouple);
bool	 pico_MAX31855_readCelsius(struct pico_MAX31855* thermocouple);
bool	 pico_MAX31855_readFahrenheit(struct pico_MAX31855* thermocouple);
bool	 pico_MAX31855_readError(struct pico_MAX31855* thermocouple);
void	 pico_MAX31855_setFaultChecks(struct pico_MAX31855* thermocouple, uint8_t faults);
uint32_t pico_MAX31855_spiread32(struct pico_MAX31855* thermocouple);
void	 pico_MAX31855_cs_select(struct pico_MAX31855* thermocouple);
void	 pico_MAX31855_cs_deselect(struct pico_MAX31855* thermocouple);
