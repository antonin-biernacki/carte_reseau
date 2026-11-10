#ifndef _PROTOCOLANTIQUE_
#define _PROTOCOLANTIQUE_

/* Includes: */
#include <avr/io.h>
#include <string.h>

#include <LUFA/Drivers/USB/USB.h>

#include "EthernetProtocols.h"
#include "Ethernet.h"
#include "ProtocolDecoders.h"

/* Macros: */
#define LS            0x01
#define LS_FOLLOW     0x11

#define SEND          0x02
#define SEND_FOLLOW   0x12
#define RECIEVE       0x03
#define RECIEVE_FOLLOW 0x13


#define LONGUEUR_DONNEE 1000


/* Function Prototypes: */
int16_t ProtocolAntique_ProcessProtocolAntiquePacket(void* IPHeaderInStart,
        void* HeaderInStart,
        void* HeaderOutStart);



typedef struct {
    uint8_t operation; /* 0x01 ls, 0x02 send, 0x03 recieve */
    uint8_t longeur; 
    uint8_t numero;
} ProtocolAntique_Header_t;

#endif
