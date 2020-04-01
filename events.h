#ifndef EVENTS_H
#define EVENTS_H

// Based on the software by Kurose and Ross
/* ******************************************************************
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
*/


#define BIDIRECTIONAL 0    /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */

/* possible events: */
#define  TIMER_INTERRUPT 0
#define  FROM_LAYER_5    1
#define  FROM_LAYER_3    2

#define   OFF            0
#define   ON             1

/* a "Message" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
#define MSG_DATA_SIZE 20
struct Message {
    char data[MSG_DATA_SIZE];
};

/* a packet is the data unit passed from layer 4 (students' code) to layer */
/* 3 (teacher's code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
union Packet {
    struct {
        unsigned short int seqnum;
        unsigned short int acknum;
        unsigned short int checksum;
        char payload[MSG_DATA_SIZE];
    } fields;
    unsigned short int chunk[3 + (MSG_DATA_SIZE / 2)];
} Packet;


extern float sim_time;    /* simulation sim_time */
extern float lambda;      /* arrival rate of messages from layer 5 */

/*****************************************************************************/
/* jimsrand(): return a float in range [0, 1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the */
/* system-supplied rand() function returns an int in the range [0, RAND_MAX] */
/*****************************************************************************/
float jimsrand();


/********************** Student-callable ROUTINES ***********************/
/*
The procedures described above are the ones that you will write. I have written the following routines which can be called by your routines:

starttimer(calling_entity,increment), where calling_entity is either 0 (for starting the A-side timer) or 1 (for starting the B side timer), and increment is .
stoptimer(calling_entity), where calling_entity is either 0 (for stopping the A-side timer) or 1 (for stopping the B side timer).
tolayer3(calling_entity,packet), where calling_entity is either , and packet is a structure of type pkt.
tolayer5(calling_entity,message), where calling_entity is either 0 (for A-side delivery to layer 5) or 1 (for B-side delivery to layer 5), and message is a structure of type msg. With unidirectional data transfer, you would only be calling this with calling_entity equal to 1 (delivery to the B-side).

*/

// for calls to to_layer_3(), to_layer_5(), start_timer() and stop_timer()
#define   A    0
#define   B    1

// Send a packet to the Network Layer from A to B or B to A
//   calling_entity: 0 (for the A-side send) or 1 (for the B side send)
//   packet: a packet to be sent
void to_layer_3(int AorB, union Packet packet);

// Pass data up to the Application Layer (layer 5).
//   AorB: 0 (for A-side delivery to layer 5) or 1 (for B-side delivery to layer 5)
//   data_sent: the string (array of characters) to be given to the application process
//   Note:  With unidirectional data transfer, you would only be calling this with calling_entity
//   equal to 1 (delivery to the B-side).
void to_layer_5(int AorB, char data_sent[MSG_DATA_SIZE]);

// Start a timer
//   AorB: 0 for starting an A-side timer and 1 for starting a B-side timer
//   increment: a float value indicating the amount of time that will pass before the timer interrupts
//
// A's timer should only be started (or stopped) by A-side routines, and similarly for the B-side timer.
// To give you an idea of the appropriate increment value to use:
//     a packet sent into the network takes an average of 5 time units to arrive at the other
//     side when there are no other messages in the medium.
void start_timer(int AorB, float increment);

// Stop a timer
//   AorB: 0 for stopping the A-side timer and 1 for stopping the B-side timer
void stop_timer(int AorB);

int sim();

#endif //COSC_315_RTP_PROJECT_SIMULATOR_H
