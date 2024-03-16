/*
This example use pico_MAX31855_init_adv() to init MAX31855 IC
It uses spi1, SPI baudrate 5 Mhz and board definition for
custom definition for SPI RX Pin and SPI SCK Pin.
*/

#include "pico/stdlib.h"
#include "stdio.h"
#include "../include/pico_MAX31855.h"

#define PICO_DEFAULT_SPI1_RX_PIN 12
#define PICO_DEFAULT_SPI1_SCK_PIN 10
#define PICO_DEFAULT_SPI1_CS_PIN 13


int main()
{
	const uint led_pin = PICO_DEFAULT_LED_PIN;
	double	   t_internal;
	double	   t_celsius;
	double	   t_fahrenheit;

	struct pico_MAX31855 thk0;
	pico_MAX31855_init(&thk0,
					   spi1,
					   5000000,
					   PICO_DEFAULT_SPI1_RX_PIN,
					   PICO_DEFAULT_SPI1_SCK_PIN,
					   PICO_DEFAULT_SPI1_CS_PIN);
	pico_MAX31855_begin(&thk0);

	// Initialize LED pin
	gpio_init(led_pin);
	gpio_set_dir(led_pin, GPIO_OUT);

	// Initialize chosen serial port
	stdio_init_all();

	// Loop forever
	while (true)
	{
		gpio_put(led_pin, true);

		t_internal = pico_MAX31855_readInternal(&thk0);

		if (t_internal == 255)
		{
			printf("SPI Read internal ERROR!\r\n");
		}
		else
		{
			printf("Internal temperature : %fC\r\n", t_internal);
		}

		gpio_put(led_pin, false);
		sleep_ms(1500);
		gpio_put(led_pin, true);

		t_celsius = pico_MAX31855_readCelsius(&thk0);

		if (t_celsius == 255)
		{
			printf("SPI Read Celsius ERROR!\r\n");
			pico_MAX31855_printError(&thk0);
		}
		else if (t_celsius == NAN)
		{
			printf("Read Celsius NAN!\r\n");
			pico_MAX31855_printError(&thk0);
		}
		else
		{
			printf("Celsius temperature : %fC\r\n", t_celsius);
			pico_MAX31855_printError(&thk0);
		}

		gpio_put(led_pin, false);
		sleep_ms(1500);
		gpio_put(led_pin, true);

		// printf("Celsius temperature : %fC\r\n", pico_MAX31855_readC(&thk0));

		t_fahrenheit = pico_MAX31855_readFahrenheit(&thk0);

		if (t_fahrenheit == 255)
		{
			printf("SPI Read Fahrenheit ERROR!\r\n");
			pico_MAX31855_printError(&thk0);
		}
		else if (t_fahrenheit == NAN)
		{
			printf("Read Fahrenheit NAN!\r\n");
			pico_MAX31855_printError(&thk0);
		}
		else
		{
			printf("Fahrenheit temperature : %fF\r\n", t_fahrenheit);
			pico_MAX31855_printError(&thk0);
		}
		gpio_put(led_pin, false);

		sleep_ms(1500);
		gpio_put(led_pin, false);
	}
}