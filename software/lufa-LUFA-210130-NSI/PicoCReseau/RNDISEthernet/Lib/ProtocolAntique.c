#include "ProtocolAntique.h"

int16_t ProtocolAntique_ProcessProtocolAntiquePacket(void* IPHeaderInStart,
        void* HeaderInStart,
        void* HeaderOutStart)
{
    DecodeProtocolAntiqueHeader(HeaderInStart);

    ProtocolAntique_Header_t* ProtocolAntiqueHeaderIN  = (ProtocolAntique_Header_t*)HeaderInStart;
    ProtocolAntique_Header_t* ProtocolAntiqueHeaderOUT = (ProtocolAntique_Header_t*)HeaderOutStart;

    switch (ProtocolAntiqueHeaderIN->operation) {
        case SEND:
                
            break;
        case RECIEVE:
            break;
        case LS:
            break;

            /* Return the size of the response so far */
            return sizeof(ProtocolAntique_Header_t);
    }

    return NO_RESPONSE;
}

