/*!
 * @file pico_MAX31855.h
 *
 * @mainpage MAX31855 Thermocouple library C for Raspberry Pico C SDK
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for the MAX31855 Thermocouple library C for
 * for Raspberry Pico C SDK.
 *
 * @section author Author
 *
 * Lazarewicz Julien lazaj30@gmail.com
 *
 * @section license License
 *
 * BSD license, all text above must be included in any redistribution.
 *
 */

#include "hardware/spi.h"
#include "math.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define MAX31855_LIB_VERSION "0.0.1"

#define MAX31855_FAULT_NONE (0x00)		 ///< Disable all fault checks
#define MAX31855_FAULT_OPEN (0x01)		 ///< Enable open circuit fault check
#define MAX31855_FAULT_SHORT_GND (0x02)	 ///< Enable short to GND fault check
#define MAX31855_FAULT_SHORT_VCC (0x04)	 ///< Enable short to VCC fault check
#define MAX31855_FAULT_ALL (0x07)		 ///< Enable all fault checks

#define MAX31855_FAULT_S_NONE "NO FAULT\r\n"			  ///< String for NO FAULT
#define MAX31855_FAULT_S_OPEN "FAULT OPEN\r\n"			  ///< String for FAULT OPEN
#define MAX31855_FAULT_S_SHORT_GND "FAULT SHORT GND\r\n"  ///< String for FAULT SHORT TO GND
#define MAX31855_FAULT_S_SHORT_VCC "FAULT SHORT VCC\r\n"  ///< String for FAULT SHORT TO VCC
#define MAX31855_FAULT_S_UNKNOW "FAULT UNKNOW\r\n"		  ///< String for FAULT UNKNOW

/*! \struct pico_MAX31855
 *
 * @brief Structure that contain parameters for MAX31855 SPI hardware
 *
 */

struct pico_MAX31855
{
	bool		initialized;  ///< initialized is TRUE if the driver is initialized or FALSE if not.
	uint8_t		faultMask;	  ///< faultMask is the mask for error retrieving.
	uint		cs_pin;		  ///< SPI Chip select hardware pin.
	uint		mosi_pin;	  ///< SPI MOSI hardware pin.
	uint		clk_pin;	  ///< SPI clock hardware pin.
	uint		spi_baudrate;  ///< SPI baudrate, maximum 5 Mhz.
	spi_inst_t* spi_device;	   ///< SPI instance spi0 or spi1.
};

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
						uint				  _cs);

/*!
 *
 * @brief Setup the HW
 *
 * @param thermocouple A pointer to pico_MAX31855 structure.
 * @return True if the device was successfully initialized, otherwise false.
 *
 */
bool pico_MAX31855_begin(struct pico_MAX31855* thermocouple);

/*!
 *
 * @brief  Read the internal temperature.
 * *
 * @param thermocouple A pointer to pico_MAX31855 structure.
 * @return The internal temperature in degrees Celsius.
 *
 */
double pico_MAX31855_readInternal(struct pico_MAX31855* thermocouple);

/*!
 *
 *	@brief  Read the thermocouple temperature.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *	@return The thermocouple temperature in degrees Celsius.
 *
 */
double pico_MAX31855_readCelsius(struct pico_MAX31855* thermocouple);

/*!
 *
 *	@brief  Read the thermocouple temperature.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *	@return The thermocouple temperature in degrees Fahrenheit.
 *
 */
double pico_MAX31855_readFahrenheit(struct pico_MAX31855* thermocouple);

/*!
 *
 *	@brief  Read the error state.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *	@return The error state.
 *
 */
uint8_t pico_MAX31855_readError(struct pico_MAX31855* thermocouple);

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
void pico_MAX31855_setFaultChecks(struct pico_MAX31855* thermocouple, uint8_t faults);

/*!
 *
 *	@brief  Read 4 bytes (32 bits) from breakout over SPI.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *	@return The raw 32 bit value read.
 *
 */
uint32_t pico_MAX31855_spiread32(struct pico_MAX31855* thermocouple);

/*!
 *
 *	@brief  Set SPI Chip select PIN to low.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *
 */
static inline void pico_MAX31855_cs_select(struct pico_MAX31855* thermocouple);

/*!
 *
 *	@brief  Set SPI Chip select PIN to high.
 *
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *
 */
static inline void pico_MAX31855_cs_deselect(struct pico_MAX31855* thermocouple);

/*!
 *
 *	@brief  Print on debug output the string value for MAX31855 error.
 *	@param thermocouple A pointer to pico_MAX31855 structure.
 *
 */
void pico_MAX31855_printError(struct pico_MAX31855* thermocouple);
