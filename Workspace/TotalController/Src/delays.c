/*
 * delays.c
 *
 *	The MIT License.
 *  Created on: 23.06.2018
 *      Author: Mateusz Salamon
 *      www.msalamon.pl
 *
 */
#include "main.h"
#include "tim.h"

#include "delays.h"

void Delay_us(uint16_t us)
{
	DELAY_US_TIMER.Instance->CNT = 0;
	while(DELAY_US_TIMER.Instance->CNT <= us);
}
