/*
 * delays.h
*
 *	The MIT License.
 *  Created on: 23.06.2018
 *      Author: Mateusz Salamon
 *      www.msalamon.pl
 *
 */


#ifndef DELAYS_H_
#define DELAYS_H_

//
//	Define timer set to 1 us tick
//
#define DELAY_US_TIMER	htim3

void Delay_us(uint16_t us);

#endif /* DELAYS_H_ */
