/*
 * boblight.h
 *
 *  Created on: Apr 4, 2015
 *      Author: c3ma
 */

#ifndef BOBLIGHT_H_
#define BOBLIGHT_H_

#include "ch.h"

/**
 * @var gBoblightMailboxBuffer
 * @var gBoblightMailbox
 * External mailbox interface to communicate with this server (Responsible for the dynamic fc protocol)
 */
extern uint32_t*	gBoblightMailboxBuffer;
extern Mailbox *	gBoblightMailbox;

void boblight_init(void);

/**
 *@fn int boblight_alive(void)
 *@return TRUE, if boblight is still running
 *@return FALSE if the boblight thread hangs
 */
int boblight_alive(void);

#endif /* BOBLIGHT_H_ */
