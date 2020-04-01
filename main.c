#include <stdio.h>
#include <stdlib.h>

/*******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).

   Edited by David Sykes, March 2018. Updated C function parameter syntax
   and renamed some functions and variables. Added macro MSG_DATA_SIZE
**********************************************************************/
#include "rtp.h"

#include "events.h"


int main(int argc, const char **argv) {
    return sim();
}