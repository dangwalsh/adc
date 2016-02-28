/*
 * bsp.c
 *
 *  Created on: Feb 27, 2016
 *      Author: danielwalsh
 */

#include "TM4C123GH6PM.h"
#include "bsp.h"

__attribute__((naked)) void assert_failed (char const *file, int line) {
    /* TBD: damage control */
    NVIC_SystemReset(); /* reset the system */
}
