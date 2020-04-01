#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include "events.h"
#include "rtp.h"

// Created by Chase, Ivan, and Sunny

/*
*     +---------+
*     | Layer 5 |    <- to_layer_5(A|B, message) passes message UP to layer 5
*     +---------+
*     | Layer 4 |
*     +---------+
*     | Layer 3 |    <- to_layer_3(A|B, packet) passes a packet to be sent to A|B
*     +---------+
*     | Layer 2 |
*     +---------+
*     | Layer 1 |
*     +---------+
*/

// GLOBAL VARIABLES

#define VERBOSE 1

static int next_packet_seq_nbr = 0;

union Packet dup_packet_A;   // duplicate of last packet sent from A
int last_seq_nbr_A = 0;

#define INTERVAL 50

// END GLOBAL VARIABLES


// UTILITY FUNCTIONS:
// Return the checksum for a packet
static unsigned short int checksum_for(union Packet packet);

// Print a message on the standard output stream
static void print_message(struct Message message);

// Print a packet on the standard output stream
static void print_packet(union Packet packet);


/********* STUDENTS WRITE THE NEXT EIGHT ROUTINES *********/
/*
* Note: A is the sender, B the receiver.
*/


static unsigned short int checksum_for(union Packet packet) {
    unsigned int sum = 0;           // 32-bit sum
    unsigned short int result;

    for (int k = 0; k < 3 + (MSG_DATA_SIZE / 2); ++k) {
        sum += packet.chunk[k];
        if (sum > 65535) {
            sum++;
            sum &= 0xffff;
        }
    }

    result = ~(unsigned short int)(sum);
    return result;
}


/* called from layer 5, passed the data to be sent to other side */
/*
* This routine will be called whenever the upper layer at the
* sending side (A) has a message to send. It is the job of
* your protocol to insure that the data in such a message
* is delivered in-order, and correctly, to the receiving
* side upper layer.
*/
void A_output(struct Message message) {
    if (VERBOSE) {
        printf("%f: A_output(", sim_time);
        print_message(message);
        printf(")\n");
    }
    // Build a packet from message
    unsigned short int seqnum =! next_packet_seq_nbr;
    unsigned short int acknum = 0;
    unsigned short int checksum = 0;
    // Load msg into packet
    union Packet packet = {{seqnum, acknum, checksum, ""}};
    for (int i = 0; i < MSG_DATA_SIZE; i++) {
        packet.fields.payload[i] = message.data[i];
    }
    checksum = checksum_for(packet);
    packet.fields.checksum = checksum;
//    if (packet.fields.payload != "ACK_________________" && packet.fields.checksum != 0) {
//        printf(packet.fields.checksum);
//        printf("No Ack");
//        return;
//    }

    // Transport the packet to Host B
    if (VERBOSE) {
        printf("Time %f: to_layer_3(B, ", sim_time);
        print_packet(packet);
        printf(")\n");
    }
        dup_packet_A = packet;
        to_layer_3(A, packet);
        start_timer(A, INTERVAL);

}

void B_output(struct Message message)  /* need be completed only for extra credit */{
    if (VERBOSE) {
        printf("Time %f: B_output(", sim_time);
        print_message(message);
        printf(")\n");
    }
    // TODO
}

/* called from layer 3, when a packet arrives for layer 4 */
/*
* This routine will be called whenever a packet sent from the B-side
* (i.e., as a result of a to_layer_3() being done by a B-side procedure)
* arrives at the A-side. packet is the (possibly corrupted) packet
* sent from the B-side.
*/
void A_input(union Packet packet) {
    if (VERBOSE) {
        printf("Time %f: A_input(", sim_time);
        print_packet(packet);
        printf(")\n");
    }

    // Check for corruption
    // ACK or NAK?
    if (checksum_for(packet) == 0 || packet.fields.payload == "ACK_________________") {
        stop_timer(A);
        char data[MSG_DATA_SIZE];
        for (int i = 0; i < MSG_DATA_SIZE; i++) {
            data[i] = packet.fields.payload[i];
        }
        to_layer_5(A, data);
        next_packet_seq_nbr =! next_packet_seq_nbr;
    }
}

/* called when A's timer goes off */
/*
* This routine will be called when A's timer expires (thus generating
* a timer interrupt). You'll probably want to use this routine to
* control the retransmission of packets.
* See start_timer() and stop_timer() below for how the timer is
* started and stopped.
*/
void A_timer_interrupt() {
    if (VERBOSE) {
        printf("%f: A_timer_interrupt()\n", sim_time);
    }
    //stop_timer(A);
    if (VERBOSE) {
        printf("%f: Resending packet ", sim_time);
        print_packet(dup_packet_A);
        printf("\n");
    }
    to_layer_3(A, dup_packet_A);
    start_timer(A, INTERVAL);
}

/* The following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
/*
* This routine will be called once, before any of your other A-side
* routines are called. It can be used to do any required initialization.
*/
void A_init() {
    if (VERBOSE) {
        printf("%f: A_init()\n", sim_time);
    }

    // Test checksum -- code for debugging
//    if (VERBOSE) {
//        printf("Testing checksum_for a packet...\n");
//
////        union Packet p = {{1, 1, 0, {0x40, 0x00, 0x40, 0x00,
////                                     0x40, 0x00, 0x40, 0x00,
////                                     0x40, 0x00, 0x40, 0x00,
////                                     0x40, 0x00, 0x40, 0x00,
////                                     0x40, 0x00, 0x40, 0x00}}};
//
//        // Note how the checksum field starts at 0
//        union Packet q = {{0x0004,
//                                  0xfffe,
//                                  0x0000,
//                                  {0x69, 0x6e, 0x64, 0x69, 0x73, 0x74, 0x69, 0x6e, 0x67, 0x75,
//                                          0x69, 0x73, 0x68, 0x61, 0x62, 0x69, 0x6c, 0x69, 0x74, 0x79}}};
//        q.fields.checksum = checksum_for(q);
//        print_packet(q);
//        printf("\n*** Checksum: %hu\n", checksum_for(q));
//    }
//    union Packet packet = {{1,0,0,{""}}};
//    to_layer_3(A,packet);
    //start_timer(A, 5);
}


/* Note that with simplex transfer from A-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B */
/*
* This routine will be called whenever a packet sent from the A-side
* (i.e., as a result of a to_layer_3() being done by a A-side procedure)
* arrives at the B-side. packet is the (possibly corrupted) packet sent
* from the A-side.
*/
void B_input(union Packet packet) {
    if (VERBOSE) {
        printf("%f: B_input(", sim_time);
        print_packet(packet);
        printf(")\n");
    }

    if (VERBOSE && checksum_for(packet) != 0) {
        printf("CHECKSUM ERROR: ");
        print_packet(packet);
        putchar('\n');
    }

    // Assume no corruption or packet loss
    // Send an ACK
   //     stop_timer(B);
    if (checksum_for(packet) == 0 || packet.fields.payload == "ACK_________________") {
        union Packet ack_packet;
        ack_packet.fields.seqnum = packet.fields.seqnum;
        ack_packet.fields.acknum = packet.fields.seqnum;
        ack_packet.fields.checksum = 0;
        memcpy(ack_packet.fields.payload, "ACK_________________", 20);
        ack_packet.fields.checksum = checksum_for(ack_packet);
        to_layer_3(B, ack_packet);

        to_layer_5(B, packet.fields.payload);
    }
}

/* called when B's timer goes off */
void B_timer_interrupt() {
    if (VERBOSE) {
        printf("%f: A_timer_interrupt()\n", sim_time);
    }
    start_timer(B,INTERVAL);
    to_layer_3(B, dup_packet_A);
}


/*
* This routine will be called once, before any of your other B-side
* routines are called. It can be used to do any required initialization.
*/
void B_init() {
    if (VERBOSE) {
        printf("%f: B_init()\n", sim_time);
    }
//    union Packet packet = {{0,1,0,{""}}};
//    to_layer_5(B,packet.fields.payload);
    //start_timer(B, 5);
}


// STATIC UTILITY FUNCTIONS

static void print_message(struct Message message) {
    printf("Message[\"");
    for (int i = 0; i < MSG_DATA_SIZE; ++i) {
        printf("%c", isprint(message.data[i]) ? message.data[i] : '`');
    }
    printf("\"]");
}

static void print_packet(union Packet packet) {
    printf("[#%d| %d 0x%04x ", packet.fields.seqnum,
           packet.fields.acknum,
           packet.fields.checksum);
    putchar('"');
    for (int i = 0; i < MSG_DATA_SIZE; ++i) {
        printf("%c", isprint(packet.fields.payload[i]) ? packet.fields.payload[i] : '`');
    }
    printf("\"]");
}






