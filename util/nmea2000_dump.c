/* Originates from A simple SocketCAN example */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdlib.h>
#include <sys/time.h>

int soc;
int read_can_port;

static struct timeval global_now; /* Global time  - lazy solution*/

int open_port(const char *port)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    /* open socket */
    soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(soc < 0)
    {
        return (-1);
    }

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, port);

    if (ioctl(soc, SIOCGIFINDEX, &ifr) < 0)
    {

        return (-1);
    }

    addr.can_ifindex = ifr.ifr_ifindex;

    fcntl(soc, F_SETFL, O_NONBLOCK);

    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {

        return (-1);
    }

    return 0;
}

void printtime(struct timeval now)
{
	static struct timeval prevTime;
	static int offset = -1;

	struct timeval nextprev;
	
	if (offset == -1) {
		// First time 
		offset = now.tv_sec;
	}
	now.tv_sec -= offset;
	nextprev = now;
	if (now.tv_usec < prevTime.tv_usec) {
		now.tv_usec += 1000000;
		now.tv_sec--;
	}
	
	printf ("%03d.%06d, %03d.%06d ", 
		(int)nextprev.tv_sec, (int)nextprev.tv_usec, 
		(int)(now.tv_sec-prevTime.tv_sec), (int)(now.tv_usec - prevTime.tv_usec));

	prevTime = nextprev;
}

void printsystime(void) 
{
	gettimeofday(&global_now, NULL);
	printtime(global_now);
}

static int lineno = 0;
void printPacket(struct can_frame canMsg)
{
	printf("%08d ", ++lineno);
	printf("%08x ", canMsg.can_id);
	unsigned i;
	for (i = 0; i < canMsg.can_dlc; i++)
	{
		printf("%02x ", canMsg.data[i]);
	}
	for (; i < 8; i++)
	{
		printf("   ");
	}
}

struct fullpgn {
	unsigned len;
	unsigned total;
	unsigned sequence;
	unsigned datalen;
	unsigned nextid;
	unsigned char data[1];
};

enum {
  PACKET_COMPLETE,
  PACKET_NOT_COMPLETE,
  PACKET_TOO_LONG,
  PACKET_OUT_OF_SEQUENCE,
  PACKET_UNEXPECTED_ID
}  add_frame(struct fullpgn* packet, struct can_frame canMsg)
{
	unsigned sequence = canMsg.data[0] >> 5;
	unsigned id = canMsg.data[0] & 0x1F;
	if ( id == 0 ) {
		// new packet
		if (canMsg.data[1] > packet->datalen) return PACKET_TOO_LONG;
		packet->len = 6;
		memcpy(packet->data, &canMsg.data[2], 6); 
		packet->total = canMsg.data[1];
		packet->sequence = sequence;
		packet->nextid = 1;
	} else {
		// fragment
		if (sequence != packet->sequence) return PACKET_OUT_OF_SEQUENCE;
		if (id != packet->nextid) return PACKET_UNEXPECTED_ID;
		packet->nextid++;
		memcpy(&packet->data[packet->len], &canMsg.data[1], 7);
		packet->len += 7;
	}
	if (packet->total <= packet->len) return PACKET_COMPLETE;

	return PACKET_NOT_COMPLETE;
}

void print129038(struct can_frame canMsg)
{
	static struct fullpgn* packet;

	if (packet == NULL) {
		packet = (struct fullpgn*) malloc(sizeof(struct fullpgn) + 26);
		packet->datalen = 27;
	}

	switch(add_frame(packet, canMsg)) {
		case PACKET_COMPLETE: {
			unsigned mmsi = *(unsigned*) (&packet->data[1]);
			unsigned lat = *(unsigned*) (&packet->data[5]);
			unsigned longitude = *(unsigned*) (&packet->data[9]);
			unsigned short cog = *(unsigned short*) (&packet->data[14]);
			unsigned short sog = *(unsigned short*) (&packet->data[16]);
			unsigned short hdg = *(unsigned short*) (&packet->data[21]);

			printf("\n");
			printf("\tmmsi(%08x) : %d\n", mmsi, mmsi);
			printf("\tlat(%08x) : %f\n", lat, (double)lat/10000000);
			printf("\tlong(%08x) : %f\n", longitude, (double)longitude/10000000);
			printf("\tcog(%04x) : %f\n", cog, (180/3.1427)*(double)cog/10000);
			printf("\tsog(%04x) : %f\n", sog, (double)sog/10000);
			printf("\thdg(%04x) : %f\n", hdg, (180/3.1427)*(double)hdg/10000);
			break;
		}
		default:
			printf("\tError detected\n");
			break;
	}
}

void print129039(struct can_frame canMsg) 
{
	static struct fullpgn* packet;

	if (packet == NULL) {
		packet = (struct fullpgn*) malloc(sizeof(struct fullpgn) + 26);
		packet->datalen = 27;
	}

	switch(add_frame(packet, canMsg)) {
		case PACKET_COMPLETE: {
			unsigned mmsi = *(unsigned*) (&packet->data[1]);
			unsigned lat = *(unsigned*) (&packet->data[5]);
			unsigned longitude = *(unsigned*) (&packet->data[9]);
			unsigned short cog = *(unsigned short*) (&packet->data[14]);
			unsigned short sog = *(unsigned short*) (&packet->data[16]);
			unsigned short hdg = *(unsigned short*) (&packet->data[21]);

			printf("\n");
			printf("\tmmsi(%08x) : %d\n", mmsi, mmsi);
			printf("\tlat(%08x) : %f\n", lat, (double)lat/10000000);
			printf("\tlong(%08x) : %f\n", longitude, (double)longitude/10000000);
			printf("\tcog(%04x) : %f\n", cog, (180/3.1427)*(double)cog/10000);
			printf("\tsog(%04x) : %f\n", sog, (double)sog/10000);
			printf("\thdg(%04x) : %f\n", hdg, (180/3.1427)*(double)hdg/10000);
			break;
		}
		default:
			printf("\tError detected\n");
			break;
	}
}

void print129794(struct can_frame canMsg)
{
	static struct fullpgn* packet;

	if (packet == NULL) {
		packet = (struct fullpgn*) malloc(sizeof(struct fullpgn) + 49);
		packet->datalen = 50;
	}

	switch(add_frame(packet, canMsg)) {
		case PACKET_COMPLETE: {
			unsigned mmsi = *(unsigned*) (&packet->data[1]);
			unsigned imo = *(unsigned*) (&packet->data[5]);
			unsigned char callsign[8];
			memcpy(callsign, &packet->data[9],7);
			callsign[8] = '\0';
			unsigned char name[21];
			memcpy(name, &packet->data[16],20);
			name[20] = '\0';

			printf("\n");
			printf("\tmmsi(%08x) : %d\n", mmsi, mmsi);
			printf("\timo(%08x) : %d\n", imo, imo);
			printf("\tcallsign: %s\n", callsign);
			printf("\tname: %s\n", name);

			break;
		}
		default:
			printf("\tError detected\n");
			break;
	}
}

void print129809(struct can_frame canMsg)
{
	static struct fullpgn* packet;

	if (packet == NULL) {
		packet = (struct fullpgn*) malloc(sizeof(struct fullpgn) + 49);
		packet->datalen = 50;
	}

	switch(add_frame(packet, canMsg)) {
		case PACKET_COMPLETE: {
			unsigned mmsi = *(unsigned*) (&packet->data[1]);
			unsigned char name[21];
			memcpy(name, &packet->data[5],20);
			name[20] = '\0';

			printf("\n");
			printf("\tmmsi(%08x) : %d\n", mmsi, mmsi);
			printf("\tname: %s\n", name);

			break;
		}
		default:
			printf("\tError detected\n");
			break;
	}
}

void print129810(struct can_frame canMsg)
{
	static struct fullpgn* packet;

	if (packet == NULL) {
		packet = (struct fullpgn*) malloc(sizeof(struct fullpgn) + 49);
		packet->datalen = 50;
	}

	switch(add_frame(packet, canMsg)) {
		case PACKET_COMPLETE: {
			unsigned mmsi = *(unsigned*) (&packet->data[1]);
			unsigned char callsign[8];
			memcpy(callsign, &packet->data[6],7);
			callsign[8] = '\0';
			unsigned char vendor[13];
			memcpy(vendor, &packet->data[16],7);
			vendor[8] = '\0';

			printf("\n");
			printf("\tmmsi(%08x) : %d\n", mmsi, mmsi);
			printf("\tcallsign: %s\n", callsign);
			printf("\tvendor: %s\n", vendor);

			break;
		}
		default:
			printf("\tError detected\n");
			break;
	}
}

typedef void (*printDetailFunc)(struct can_frame canMsg);


const struct pgnPair {
	unsigned int pgn;
	char *name;
	printDetailFunc detail;
} pgns[] =	 {
	{ 65280,  "unknown", NULL }, 
	{ 65305,  "Unknown", NULL }, 
	{ 126992, "System Time", NULL },
	{ 127250, "Vessel Heading", NULL, },
	{ 127258, "Magnetic variation", NULL },
	{ 128259, "speed", NULL },
	{ 128267, "Water depth", NULL },
	{ 128275, "Distance Log", NULL },
	{ 129025, "Position, Rapid", NULL },
	{ 129026, "COG & SOG, Rapid", NULL },
	{ 129029, "GNSS Position Data", NULL },
	{ 129033, "Time and Date", NULL },
	{ 129038, "Pos Report Class A" , print129038}, 
	{ 129039, "Pos Report Class B" , print129039},
	{ 129283, "Cross Tracke error", NULL },
	{ 129284, "Navigation data", NULL },
	{ 129285, "Nav: Route/WP information", NULL },
	{ 129539, "GNSS DOPs", NULL }, 
	{ 129540, "GNSS Sats in View" , NULL},
	{ 129793, "STATIC_AND_VOYAGE" , NULL},
	{ 129794, "AIS Class A, Static and Voyage", print129794 },
	{ 129809, "Static Data Report Part A", print129809 },
	{ 129810, "Static Data Report Part B" , print129810 },
	{ 130310, "Environmental parameters" , NULL}, 
	{ 130311, "Environmental parameters" , NULL},
	{ 130312, "unknown", NULL },
	{ 130822, "unknown" , NULL},
	{ 130824, "unknown" , NULL},
	{ 0, "unhandled" , NULL},
};

void printPgn(unsigned pgn, struct can_frame canMsg)
{
	const struct pgnPair* next = &pgns[0];
	while (next->pgn != 0) {
		if (pgn ==next->pgn) {
			printf("%s", next->name);
			if (next->detail)
				next->detail(canMsg);
			return;
		}
		next++;
	}
	printf("%s", next->name);
}

void printCanMsg(struct can_frame canMsg, unsigned int timestamp)
{
	unsigned pgn = canMsg.can_id >> 8;
	pgn &= 0x1FFFF;
	printPacket(canMsg);
	printf("(PGN %d) ", pgn);
	printPgn(pgn, canMsg);
	printf("\n");
}

void read_stdin(void)
{
	size_t size;
	char* line=NULL;
	FILE* fd = fopen("out2.txt", "r");
	while(getline(&line, &size,fd) >= 0) {
		struct can_frame canMsg;
		unsigned int data[8];
		int len;
		unsigned int canid;
		int idx = 0;
		unsigned int sec;
		unsigned int usec;
		char payloadStr[100];
		memset(payloadStr,0,100);

		int cnt = sscanf(line, "%d.%d %*d %xx Rx d %d %23c",
				&sec, &usec, &canid, &len, payloadStr);
		sscanf(payloadStr, "%x %x %x %x %x %x %x %x",
				&data[0], &data[1], &data[2], &data[3],
				&data[4], &data[5], &data[6], &data[7]);
		canMsg.can_id = canid;
		canMsg.can_dlc = len;
	//	printf ("sec=%d, usec=%06d", sec, usec);
		usec += sec*1000000;
//		printf(" -> %d\n", usec);
		while(len) {
			canMsg.data[idx] = data[idx];
			idx++;
			len--;
		}
		printCanMsg(canMsg, usec);
	}
	free(line);
	fclose(fd);
}


/* this is just an example, run in a thread */
void read_port()
{
    struct can_frame frame_rd;
    int recvbytes = 0;

    read_can_port = 1;
    while(read_can_port)
    {
        struct timeval timeout = {1, 0};
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(soc, &readSet);

        if (select((soc + 1), &readSet, NULL, NULL, &timeout) >= 0)
        {
            if (!read_can_port)
            {
                break;
            }
            if (FD_ISSET(soc, &readSet))
            {
                recvbytes = read(soc, &frame_rd, sizeof(struct can_frame));
                if(recvbytes)
                {
                    printCanMsg(frame_rd, 0);

                }
            }
        }

    }

}


int close_port()
{
    close(soc);
    return 0;
}

int main(int argc, char* argv[])
{
   if (argc == 2 && *(argv[1]) == '-') {
	char option[100];
	strcpy(option, argv[1]);
	if (strcmp(option, "-v") == 0) {
		printf("Expecting stdin\n");
		read_stdin();
	}
    } else {
      open_port("can0");
      read_port();
    }
    return 0;
}
