/*!
 * @file pico_MAX31855.cpp
 *
 * @mainpage Adafruit MAX31855 Thermocouple Breakout Driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's MAX31855 thermocouple breakout
 * driver for the Arduino platform.  It is designed specifically to work with
 * the Adafruit MAX31855 breakout: https://www.adafruit.com/products/269
 *
 * This breakout uses SPI to communicate, 3 pins are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a
 * href="https://github.com/adafruit/Adafruit_BusIO"> Adafruit_BusIO</a> being
 * present on your system. Please make sure you have installed the latest
 * version before using this library.
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text above must be included in any redistribution.
 *
 */

#include "pico_CMAX31855.h"

/**************************************************************************/
/*!
	@brief  Instantiates a new pico_MAX31855 class using software SPI.

	@param _sclk The pin to use for SPI Serial Clock.
	@param _cs The pin to use for SPI Chip Select.
	@param _miso The pin to use for SPI Master In Slave Out.
*/
/**************************************************************************/
/* pico_MAX31855::pico_MAX31855(int8_t _sclk, int8_t _cs, int8_t _miso)
: spi_dev(_cs, _sclk, _miso, -1, 1000000)
{
} */

/**************************************************************************/
/*!
	@brief  Instantiates a new pico_MAX31855 class using hardware SPI.

	@param _cs The pin to use for SPI Chip Select.
	@param _spi which spi buss to use.
*/
/**************************************************************************/
bool pico_MAX31855_init(struct pico_MAX31855* thermocouple, uint _cs)
{
	thermocouple->cs_pin	  = _cs;
	thermocouple->initialized = false;
	thermocouple->faultMask	  = MAX31855_FAULT_ALL;
	return true;
}

/**************************************************************************/
/*!
	@brief  Setup the HW

	@return True if the device was successfully initialized, otherwise false.
*/
/**************************************************************************/
bool pico_MAX31855_begin(struct pico_MAX31855* thermocouple)
{
	spi_init(PICO_DEFAULT_SPI, 500 * 1000);
	gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	// Make the SPI pins available to picotool
	// bi_decl(bi_3pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN,
	// PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));

	// Chip select is active-low, so we'll initialise it to a driven-high state
	gpio_init(thermocouple->cs_pin);
	gpio_set_dir(thermocouple->cs_pin, GPIO_OUT);
	gpio_put(thermocouple->cs_pin, 1);
	// Make the CS pin available to picotool
	// bi_decl(bi_1pin_with_name(thermocouple->cs_pin, "SPI CS"));

	thermocouple->initialized = true;

	return thermocouple->initialized;
}

/**************************************************************************/
/*!
	@brief  Read the internal temperature.

	@return The internal temperature in degrees Celsius.
*/
/**************************************************************************/
bool pico_MAX31855_readInternal(struct pico_MAX31855* thermocouple)
{
	uint32_t v;

	v = pico_MAX31855_spiread32(thermocouple);

	if (v == 0)
	{
		return false;
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
	thermocouple->t_internal = internal;

	return thermocouple->t_internal;
}

/**************************************************************************/
/*!
	@brief  Read the thermocouple temperature.

	@return The thermocouple temperature in degrees Celsius.
*/
/**************************************************************************/
bool pico_MAX31855_readCelsius(struct pico_MAX31855* thermocouple)
{
	int32_t v;

	v = pico_MAX31855_spiread32(thermocouple);

	if (v == 0)
	{
		return false;
	}

	// Serial.print("0x"); Serial.println(v, HEX);

	/*
	float internal = (v >> 4) & 0x7FF;
	internal *= 0.0625;
	if ((v >> 4) & 0x800)
	  internal *= -1;
	Serial.print("\tInternal Temp: "); Serial.println(internal);
	*/

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
	// Serial.println(v, HEX);

	double centigrade = v;

	// LSB = 0.25 degrees C
	centigrade *= 0.25;

	thermocouple->t_celsius = centigrade;

	return true;
}

/**************************************************************************/
/*!
	@brief  Read the error state.

	@return The error state.
*/
/**************************************************************************/
bool pico_MAX31855_readError(struct pico_MAX31855* thermocouple)
{
	thermocouple->error = pico_MAX31855_spiread32(thermocouple) & 0x7;
	return true;
}

/**************************************************************************/
/*!
	@brief  Read the thermocouple temperature.

	@return The thermocouple temperature in degrees Fahrenheit.
*/
/**************************************************************************/
bool pico_MAX31855_readFahrenheit(struct pico_MAX31855* thermocouple)
{
	float f = 0;
	pico_MAX31855_readCelsius(thermocouple);
	f = thermocouple->t_celsius;
	f *= 9.0;
	f /= 5.0;
	f += 32;
	thermocouple->t_fahrenheit = f;

	return true;
}

/**************************************************************************/
/*!
	@brief  Set the faults to check when reading temperature. If any set
	faults occur, temperature reading will return NAN.

	@param faults Faults to check. Use logical OR combinations of preset
	fault masks: MAX31855_FAULT_OPEN, MAX31855_FAULT_SHORT_GND,
	MAX31855_FAULT_SHORT_VCC. Can also enable/disable all fault checks
	using: MAX31855_FAULT_ALL or MAX31855_FAULT_NONE.
*/
/**************************************************************************/
void pico_MAX31855_setFaultChecks(struct pico_MAX31855* thermocouple, uint8_t faults)
{
	thermocouple->faultMask = faults & 0x07;
}

/**************************************************************************/
/*!
	@brief  Read 4 bytes (32 bits) from breakout over SPI.

	@return The raw 32 bit value read.
*/
/**************************************************************************/
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
	if (spi_is_readable(PICO_DEFAULT_SPI))
	{
		spi_read_blocking(PICO_DEFAULT_SPI, 0, buf, 4);
	}
	else
	{
		return 0;
	}
	pico_MAX31855_cs_deselect(thermocouple);
	sleep_ms(1);

	d = buf[0];
	d <<= 8;
	d |= buf[1];
	d <<= 8;
	d |= buf[2];
	d <<= 8;
	d |= buf[3];

	// Serial.println(d, HEX);

	return d;
}

void pico_MAX31855_cs_select(struct pico_MAX31855* thermocouple)
{
	asm volatile("nop \n nop \n nop");
	gpio_put(thermocouple->cs_pin, 0);	// Active low
	asm volatile("nop \n nop \n nop");
}

void pico_MAX31855_cs_deselect(struct pico_MAX31855* thermocouple)
{
	asm volatile("nop \n nop \n nop");
	gpio_put(thermocouple->cs_pin, 1);
	asm volatile("nop \n nop \n nop");
}