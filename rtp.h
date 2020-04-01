//
// Created by David A. Sykes on 3/21/18.
// Modified by David A. Sykes on 10/14/19.
//

#ifndef RTP_H
#define RTP_H

#include "events.h"

/*
 * A is the sender, B the receiver.
 */

/* called from layer 5, passed the data to be sent to other side */
/*
 * This routine will be called whenever the upper layer at the
 * sending side (A) has a message to send. It is the job of
 * your protocol to insure that the data in such a message
 * is delivered in-order, and correctly, to the receiving
 * side upper layer.
 */
void A_output(struct Message message);

void B_output(struct Message message);  /* need be completed only for extra credit */


/* called from layer 3, when a packet arrives for layer 4 */
/*
 * This routine will be called whenever a packet sent from the B-side
 * (i.e., as a result of a tolayer3() being done by a B-side procedure)
 * arrives at the A-side. packet is the (possibly corrupted) packet
 * sent from the B-side.
 */
void A_input(union Packet packet);


/* called when A's timer goes off */
/*
 * This routine will be called when A's timer expires (thus generating
 * a timer interrupt). You'll probably want to use this routine to
 * control the retransmission of packets.
 * See start_timer() and stop_timer() below for how the timer is
 * started and stopped.
 */
void A_timer_interrupt();


/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
/*
 * This routine will be called once, before any of your other A-side
 * routines are called. It can be used to do any required initialization.
 */
void A_init();


/* Note that with simplex transfer from A-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
/*
 * This routine will be called whenever a packet sent from the A-side
 * (i.e., as a result of a to_layer_3() being done by a A-side procedure)
 * arrives at the B-side. packet is the (possibly corrupted) packet sent
 * from the A-side.
 */
void B_input(union Packet packet);


/* called when B's timer goes off */
void B_timer_interrupt();


/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
/*
 * This routine will be called once, before any of your other B-side
 * routines are called. It can be used to do any required initialization.
 */
void B_init();

#endif //COSC_315_RTP_PROJECT_RTP_H
