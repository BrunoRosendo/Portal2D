#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <lcom/lcf.h>

/** @defgroup protocol protocol
 * @{
 *
 * Constants defining the serial port's protocol used in the project
 */


#define P_MESSAGE_END 0xFF
#define P_CONNECT 0xF0

#define P_ACK 0xFA  // same as PS2
#define P_NACK 0xFE

#define P_DELAY 10000

// messages to send
#define P_KBC 0xF1
#define P_NEW_PORTAL 0xF2


#endif
