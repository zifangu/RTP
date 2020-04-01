//
// Created by Sykes, David A on 3/21/18.
// Based on code from Kurose and Ross.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "events.h"
#include "rtp.h"


struct event {
    float evtime;           /* event sim_time */
    int evtype;             /* event type code */
    int eventity;           /* entity where event occurs */
    union Packet *pktptr;  /* ptr to packet (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};


/********************* EVENT HANDLING ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/
static void init();
static void generate_next_arrival();
static void insert_event(struct event *p);
//static void print_evlist();


/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
  - emulates the transmission and delivery (possibly with bit-level corruption
    and packet loss) of packets across the layer 3/4 interface
  - handles the starting/stopping of a timer, and generates timer
    interrupts (resulting in calling students timer handler).
  - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOULD NOT TOUCH OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you definitely should not have to modify
******************************************************************/

struct event *evlist = NULL;   /* the event list */

#include "events.h"

int TRACE = 1;             /* for my debugging */
int n_sim = 0;             /* number of messages from 5 to 4 so far */
int n_sim_max = 0;         /* number of msgs to generate, then stop */
float sim_time = 0.0;
float loss_prob;           /* probability that a packet is dropped  */
float corrupt_prob;        /* probability that one bit is packet is flipped */
float lambda;              /* arrival rate of messages from layer 5 */
int n_to_layer_3;          /* number sent into layer 3 */
int n_lost;                /* number lost in media */
int n_corrupt;             /* number corrupted by media*/


int sim() {
    struct event *event_ptr;
    struct Message msg2give;
    union Packet pkt2give;

    int i, j;
    // char c;

    init();
    A_init();
    B_init();

    while (1) {
        event_ptr = evlist;            /* get next event to simulate */
        if (event_ptr == NULL)
            goto terminate;
        evlist = evlist->next;        /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE >= 2) {
            printf("\nEVENT sim_time: %f,", event_ptr->evtime);
            printf("  type: %d", event_ptr->evtype);
            if (event_ptr->evtype == 0)
                printf(", timer interrupt  ");
            else if (event_ptr->evtype == 1)
                printf(", from layer 5 ");
            else
                printf(", from layer 3 ");
            printf(" entity: %d\n", event_ptr->eventity);
        }
        sim_time = event_ptr->evtime;        /* update sim_time to next event sim_time */
        if (n_sim == n_sim_max)
            break;                        /* all done with simulation */
        if (event_ptr->evtype == FROM_LAYER_5) {
            generate_next_arrival();   /* set up future arrival */
            /* fill in Message to give with string of same letter */
            j = n_sim % 26;
            for (i = 0; i < MSG_DATA_SIZE; i++)
                msg2give.data[i] = (char)(97 + j);
            if (TRACE > 2) {
                printf("          MAINLOOP: data given to student: ");
                for (i = 0; i < MSG_DATA_SIZE; i++)
                    printf("%c", msg2give.data[i]);
                printf("\n");
            }
            n_sim++;
            if (event_ptr->eventity == A)
                A_output(msg2give);
            else
                B_output(msg2give);
        } else if (event_ptr->evtype == FROM_LAYER_3) {
            pkt2give.fields.seqnum = event_ptr->pktptr->fields.seqnum;
            pkt2give.fields.acknum = event_ptr->pktptr->fields.acknum;
            pkt2give.fields.checksum = event_ptr->pktptr->fields.checksum;
            for (i = 0; i < MSG_DATA_SIZE; i++)
                pkt2give.fields.payload[i] = event_ptr->pktptr->fields.payload[i];
            if (event_ptr->eventity == A)      /* deliver packet by calling */
                A_input(pkt2give);            /* appropriate entity */
            else
                B_input(pkt2give);
            free(event_ptr->pktptr);          /* free the memory for packet */
        } else if (event_ptr->evtype == TIMER_INTERRUPT) {
            if (event_ptr->eventity == A)
                A_timer_interrupt();
            else
                B_timer_interrupt();
        } else {
            printf("INTERNAL PANIC: unknown event type \n");
        }
        free(event_ptr);
    }

    terminate:
    printf(" Simulator terminated at sim_time %f\n after sending %d msgs from layer5\n", sim_time, n_sim);

    return 0;
}



void init()                         /* initialize the simulator */
{
    int i;
    float sum, avg;

    printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
    printf("Enter the number of messages to simulate: ");
    scanf("%d", &n_sim_max);
    printf("Enter  packet loss probability [enter 0.0 for no loss]:");
    scanf("%f", &loss_prob);
    printf("Enter packet corruption probability [0.0 for no corruption]:");
    scanf("%f", &corrupt_prob);
    printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
    scanf("%f", &lambda);
    printf("Enter TRACE:");
    scanf("%d", &TRACE);

    srand(9999);              /* init random number generator */
    sum = 0.0;                /* test random number generator for uniform distribution */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand();    /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75) {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(1);
    }

    n_to_layer_3 = 0;
    n_lost = 0;
    n_corrupt = 0;

    sim_time = 0.0;              /* initialize sim_time to 0.0 */
    generate_next_arrival();     /* initialize event list */
}

float jimsrand() {
//    double mmm = 2147483647; /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
    double mmm = RAND_MAX;     // Added by DAS
    float x;                   /* individual students may need to change mmm */
    x = rand() / mmm;          /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLING ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival() {
    double x, log(), ceil();
    struct event *evptr;
//    float ttime;
//    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2;  /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = (struct event *) malloc(sizeof(struct event));
    evptr->evtime = sim_time + x;
    evptr->evtype = FROM_LAYER_5;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insert_event(evptr);
}


void insert_event(struct event *p) {
    struct event *q, *qold;

    if (TRACE > 2) {
        printf("            INSERT EVENT: sim_time is %lf\n", sim_time);
        printf("            INSERT EVENT: future sim_time will be %lf\n", p->evtime);
    }
    q = evlist;     /* q points to header of list in which p struct inserted */
    if (q == NULL) {   /* list is empty */
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL) {   /* end of list */
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        }
        else if (q == evlist) { /* front of list */
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        }
        else {     /* middle of list */
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}


/*
 The function below is not currently being used.
static void print_evlist() {
    struct event *q;
//    int i;
    printf("--------------\nEvent List Follows:\n");
    for (q = evlist; q != NULL; q = q->next) {
        printf("Event sim_time: %f, type: %d entity: %d\n", q->evtime, q->evtype, q->eventity);
    }
    printf("--------------\n");
}
*/


void stop_timer(int AorB)
/* A or B is trying to stop timer */
{
    struct event *q ; // , *qold;

    if (TRACE > 2)
        printf("          STOP TIMER: stopping timer at %f\n", sim_time);
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB)) {
            /* remove this event */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL;         /* remove first and only event on list */
            else if (q->next == NULL) /* end of list - there is one in front */
                q->prev->next = NULL;
            else if (q == evlist) { /* front of list - there must be event after */
                q->next->prev = NULL;
                evlist = q->next;
            } else {     /* middle of list */
                q->next->prev = q->prev;
                q->prev->next = q->next;
            }
            free(q);
            return;
        }
    printf("Warning: unable to cancel your timer. It wasn't running.\n");
}


void start_timer(int AorB, float increment)
/* A or B is trying to stop timer */
{

    struct event *q;
    struct event *evptr;

    if (TRACE > 2)
        printf("          START TIMER: starting timer at %f\n", sim_time);
    /* be nice: check to see if timer is already started, if so, then  warn */
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (
            q = evlist;
            q != NULL;
            q = q->next
            )
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB)) {
            printf("Warning: attempt to start a timer that is already started\n");
            return;
        }

/* create future event for when timer goes off */
    evptr = (struct event *) malloc(sizeof(struct event));
    evptr->
            evtime = sim_time + increment;
    evptr->
            evtype = TIMER_INTERRUPT;
    evptr->
            eventity = AorB;
    insert_event(evptr);
}


/************************** TO_LAYER_3 ***************/
void to_layer_3(int AorB, union Packet packet)
/* A or B is trying to stop timer */
{
    union Packet *mypktptr;
    struct event *evptr, *q;

    float lastime, x, jimsrand();
    int i;

    n_to_layer_3++;

    /* simulate losses: */
    if (jimsrand() < loss_prob) {
        n_lost++;
        if (TRACE > 0)
            printf("          TO_LAYER_3: packet being lost\n");
        return;
    }

    /* make a copy of the packet student just gave me since he/she may decide */
    /* to do something with the packet after we return back to him/her */
    mypktptr = (union Packet *) malloc(sizeof(union Packet));
    mypktptr->fields.seqnum = packet.fields.seqnum;
    mypktptr->fields.acknum = packet.fields.acknum;
    mypktptr->fields.checksum = packet.fields.checksum;
    for (i = 0; i < MSG_DATA_SIZE; i++)
        mypktptr->fields.payload[i] = packet.fields.payload[i];

    if (TRACE > 2) {
        printf("          TO_LAYER_3: seq: %d, ack %d, check: %d ",
               mypktptr->fields.seqnum,
               mypktptr->fields.acknum,
               mypktptr->fields.checksum);
        for (i = 0; i < MSG_DATA_SIZE; i++)
            printf("%c", mypktptr->fields.payload[i]);
        printf("\n");
    }

    /* create future event for arrival of packet at the other side */
    evptr = (struct event *) malloc(sizeof(struct event));
    evptr->
            evtype = FROM_LAYER_3;   /* packet will pop out from layer3 */
    evptr->
            eventity = (AorB + 1) % 2; /* event occurs at other entity */
    evptr->
            pktptr = mypktptr;       /* save ptr to my copy of packet */
    /* finally, compute the arrival sim_time of packet at the other end.
        medium can not reorder, so make sure packet arrives between 1 and 10
        sim_time units after the latest arrival sim_time of packets
        currently in the medium on their way to the destination */
    lastime = sim_time;
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_LAYER_3 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 1 + 9 * jimsrand();

    /* simulate corruption: */
    if (jimsrand() < corrupt_prob) {
        n_corrupt++;
        if ((x = jimsrand()) < .75) {
            /* corrupt payload */
            int i = rand() % MSG_DATA_SIZE;   // Random character in message to corrupt
            if (TRACE > 0) {
                printf("          --- Corrupting packet payload (character %d)", i);
            }
            // mypktptr->fields.payload[0] = 'Z';
            // mypktptr->fields.payload[i] = toupper(mypktptr->fields.payload[i]);
            // mypktptr->fields.payload[i]++;
            mypktptr->fields.payload[i] ^= 0x80;  // Flip between lower and upper case
        }
        else if (x < .875)
            // Corrupt the seqnum field
            mypktptr->fields.seqnum ^= 0x0040;
        else
            // Corrupt the acknum field
            mypktptr->fields.acknum ^= 0x0040;

        if (TRACE > 0)
            printf("          TO_LAYER_3: packet being corrupted\n");
    }

    if (TRACE > 2)
        printf("          TO_LAYER_3: scheduling arrival on other side\n");
    insert_event(evptr);
}

void to_layer_5(int AorB, char data_sent[MSG_DATA_SIZE]) {
    int i;
    if (TRACE > 2) {
        printf("          TO_LAYER_5: data received: ");
        for (
                i = 0;
                i < MSG_DATA_SIZE; i++)
            printf("%c", data_sent[i]);
        printf("\n");
    }
}