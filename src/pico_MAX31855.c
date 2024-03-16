/*!
 *
 * @file pico_MAX31855.c
 *
 */

#include "../include/pico_MAX31855.h"

/*!
 *
 * @brief  Initialize a new pico_MAX31855 structure using hardware SPI.
 *
 * @param thermocouple A pointer to pico_MAX31855 structure.
 * @param _spi_device A pointer to spi_inst_t, spi0 or spi1.
 * @param _spi_baudrate Value of the SPI baudrate, maximum value 5 Mhz.
 * @param _mosi The pin to use for SPI Mosi.
 * @param _clk The pin to use for SPI Clock
 * @param _cs The pin to use for SPI Chip Select.
 *
 */
bool pico_MAX31855_init(struct pico_MAX31855* thermocouple,
						spi_inst_t*			  _spi_device,
						uint				  _spi_baudrate,
						uint				  _mosi,
						uint				  _clk,
						uint				  _cs)
{
	thermocouple->mosi_pin	   = _mosi;
	thermocouple->clk_pin	   = _clk;
	thermocouple->cs_pin	   = _cs;
	thermocouple->spi_device   = _spi_device;
	thermocouple->spi_baudrate = _spi_baudrate;
	thermocouple->initialized  = false;
	thermocouple->faultMask	   = MAX31855_FAULT_ALL;
	return true;
}

/*!
 *
 * @brief Setup the HW
 *
 * @param thermocouple A pointer to pico_MAX31855 structure.
 * @return True if the device was successfully initialized, otherwise false.
 *
 */
bool pico_MAX31855_begin(struct pico_MAX31855* thermocouple)
{
	// spi_init(spi0, 1000 * 1000);
	spi_init(thermocouple->spi_device, thermocouple->spi_baudrate);

	gpio_set_function(thermocouple->mosi_pin, GPIO_FUNC_SPI);
	gpio_set_function(thermocouple->clk_pin, GPIO_FUNC_SPI);

	// Chip select is active-low, so we'll initialise it to a driven-high state
	gpio_init(thermocouple->cs_pin);
	gpio_set_dir(thermocouple->cs_pin, GPIO_OUT);
	gpio_put(thermocouple->cs_pin, 1);

	thermocouple->initialized = true;

	return thermocouple->initialized;
}

/*!
 *
 * @brief  Read the internal temperature.
 * *
 * @param thermocouple A pointer to pico_MAX31855 structure.
 * @return The internal temperature in degrees Celsius.
 *
 */
double pico_MAX31855_readInternal(struct pico_MAX31855* thermocouple)
{
	uint32_t v;

	v = pico_MAX31855_spiread32(thermocouple);

	if (v == 0)
	{
		return 255;
	}

	// ignore bottom 4 bits - they're just thermocouple data
	v >>= 4;

	// pull the bottom 11 bits off
	float internal = v & 0x7FF;
	// check sign bit!
	if (v & 0x800)
	{
		// Convert to negative value by extending sign and casting to signed type.
		int16_t tmp = 0xF800 | (v & 0x7FF);
		internal	= tmp;
	}
	internal *= 0.0625;	 // LSB = 0.0625 degrees

	return internal;
}

/*!
 *
 *	@brief  Read the thermocouple temperature.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *	@return The thermocouple temperature in degrees Celsius.
 *
 */
double pico_MAX31855_readCelsius(struct pico_MAX31855* thermocouple)
{
	int32_t v;

	v = pico_MAX31855_spiread32(thermocouple);

	if (v == 0)
	{
		return 255;
	}

	if (v & thermocouple->faultMask)
	{
		// uh oh, a serious problem!
		return NAN;
	}

	if (v & 0x80000000)
	{
		// Negative value, drop the lower 18 bits and explicitly extend sign bits.
		v = 0xFFFFC000 | ((v >> 18) & 0x00003FFF);
	}
	else
	{
		// Positive value, just drop the lower 18 bits.
		v >>= 18;
	}

	double centigrade = v;

	// LSB = 0.25 degrees C
	centigrade *= 0.25;

	return centigrade;
}

/*!
 *
 *	@brief  Read the error state.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *	@return The error state.
 *
 */
uint8_t pico_MAX31855_readError(struct pico_MAX31855* thermocouple)
{
	uint8_t e = pico_MAX31855_spiread32(thermocouple) & 0x7;
	return e;
}

/*!
 *
 *	@brief  Read the thermocouple temperature.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *	@return The thermocouple temperature in degrees Fahrenheit.
 *
 */
double pico_MAX31855_readFahrenheit(struct pico_MAX31855* thermocouple)
{
	float f = 0;

	f = pico_MAX31855_readCelsius(thermocouple);

	if (f == 255)
	{
		return f;
	}
	else if (f == NAN)
	{
		return f;
	}
	else
	{
		f *= 9.0;
		f /= 5.0;
		f += 32;

		return f;
	}
}

/*!
 *
 *	@brief  Set the faults to check when reading temperature. If any set
 *	faults occur, temperature reading will return NAN.
 *
 * 	@param thermocouple A pointer to pico_MAX31855 structure.
 *	@param faults Faults to check. Use logical OR combinations of preset
 *	fault masks: MAX31855_FAULT_OPEN, MAX31855_FAULT_SHORT_GND,
 *	MAX31855_FAULT_SHORT_VCC. Can also enable/disable all fault checks
 *	using: MAX31855_FAULT_ALL or MAX31855_FAULT_NONE.
 *
 */
void pico_MAX31855_setFaultChecks(struct pico_MAX31855* thermocouple, uint8_t faults)
{
	thermocouple->faultMask = faults & 0x07;
}

/*!
 *
 *	@brief  Read 4 bytes (32 bits) from breakout over SPI.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *	@return The raw 32 bit value read.
 *
 */
uint32_t pico_MAX31855_spiread32(struct pico_MAX31855* thermocouple)
{
	uint32_t d = 0;
	uint8_t	 buf[4];

	// backcompatibility!
	if (!thermocouple->initialized)
	{
		pico_MAX31855_begin(thermocouple);
	}

	pico_MAX31855_cs_select(thermocouple);
	sleep_ms(1);

	// spi_read_blocking(spi0, 0, buf, 4);
	spi_read_blocking(thermocouple->spi_device, 0, buf, 4);
	pico_MAX31855_cs_deselect(thermocouple);
	sleep_ms(1);

	d = buf[0];
	d <<= 8;
	d |= buf[1];
	d <<= 8;
	d |= buf[2];
	d <<= 8;
	d |= buf[3];

	return d;
}

/*!
 *
 *	@brief  Set SPI Chip select PIN to low.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *
 */
static inline void pico_MAX31855_cs_select(struct pico_MAX31855* thermocouple)
{
	asm volatile("nop \n nop \n nop");
	gpio_put(thermocouple->cs_pin, 0);	// Active low
	asm volatile("nop \n nop \n nop");
}

/*!
 *
 *	@brief  Set SPI Chip select PIN to high.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *
 */
static inline void pico_MAX31855_cs_deselect(struct pico_MAX31855* thermocouple)
{
	asm volatile("nop \n nop \n nop");
	gpio_put(thermocouple->cs_pin, 1);
	asm volatile("nop \n nop \n nop");
}

/*!
 *
 *	@brief  Print on debug output the string value for MAX31855 error.
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *
 */
void pico_MAX31855_printError(struct pico_MAX31855* thermocouple)
{
	uint8_t e = pico_MAX31855_readError(thermocouple);
	switch (e)
	{
		case MAX31855_FAULT_NONE:
			printf(MAX31855_FAULT_S_NONE);
			break;

		case MAX31855_FAULT_OPEN:
			printf(MAX31855_FAULT_S_OPEN);
			break;

		case MAX31855_FAULT_SHORT_GND:
			printf(MAX31855_FAULT_S_SHORT_GND);
			break;

		case MAX31855_FAULT_SHORT_VCC:
			printf(MAX31855_FAULT_S_SHORT_VCC);
			break;

		default:
			printf(MAX31855_FAULT_S_UNKNOW);
			break;
	}
}