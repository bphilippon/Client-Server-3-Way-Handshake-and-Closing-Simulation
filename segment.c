#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

//TCP Segment Definition

struct TCP_segment
{
	unsigned short int sourcePort;	//Port number of segment source. 16 bits
	unsigned short int destPort;	//port number for segment definition. 16 bits
	unsigned int seqNumber;		//Designated segment sequence number. 32 bits
	unsigned int ackNumber;		//Segment acknowledgement number. 32 bits

	unsigned short int offset : 4;	//Header Length, declared as a 4 bit field

	unsigned short int reserved : 6;	//A 6-bit field of reserved space

	//Six 1-bit fielda for flags
	unsigned short int URG_flag : 1;	//Urgent flag
	unsigned short int ACK_flag : 1;	//Acknowledgement flag
	unsigned short int PSH_flag : 1;	//Push flag
	unsigned short int RST_flag : 1;	//Reset flag
	unsigned short int SYN_flag : 1;	//Synchronize flag
	unsigned short int FIN_flag : 1;	//Finish flag

	unsigned short int recvWindow;	//16-bit receive window. Initially 0
	unsigned short int checksum;	//16-bit checksum to be computed after header and data is set. Initially 0
	unsigned short int urgPtr;	//16-bit urgent pointer. Initially 0
	unsigned int options;		//32-bits for options. Initially 0
};

/* Calculates value of the checksum
*/
void calcChkSum(struct TCP_segment *segmentToCalc)
{
	struct TCP_segment tcp_seg;
	unsigned short int cksum_arr[12];
	unsigned int i, sum = 0, cksum;

	memcpy(cksum_arr, segmentToCalc, 24);	//Copying 24 bits

	for(i = 0; i < 12; i++)		//Compute sum
		sum += cksum_arr[i];

	cksum = sum >> 16;		//Fold once
	sum = sum & 0x0000FFFF;
	sum += cksum;

	cksum = sum >> 16;		//Fold once more
	sum = sum & 0x0000FFFF;
	cksum += sum;

	cksum = (0xFFFF ^ cksum);

	segmentToCalc->checksum = cksum;

	return;
}

/* Prints contents of segment to console and output file
*/
void printSegment(struct TCP_segment *segmentToPrint, FILE *output)
{
	//Print to console
	printf("        -Segment Fields-\n");
	printf("Source Port: 	        %hu\n", segmentToPrint->sourcePort);
	printf("Destination Port:       %hu\n", segmentToPrint->destPort);
	printf("Sequence Number:        %hu\n", segmentToPrint->seqNumber);
	printf("Acknowledgement Number: %u\n", segmentToPrint->ackNumber);
	printf("Offset:			%hu\n", segmentToPrint->offset);
	printf("URG Flag:		%hu\n", segmentToPrint->URG_flag);
	printf("ACK Flag:		%hu\n", segmentToPrint->ACK_flag);
	printf("PSH Flag:		%hu\n", segmentToPrint->PSH_flag);
	printf("RST Flag:		%hu\n", segmentToPrint->RST_flag);
	printf("SYN Flag:		%hu\n", segmentToPrint->SYN_flag);
	printf("FIN Flag:		%hu\n", segmentToPrint->FIN_flag);
	printf("Receive Window:		%hu\n", segmentToPrint->recvWindow);
	printf("Checksum: 		%hu\n", segmentToPrint->checksum);
	printf("Urgent Ptr:		%hu\n", segmentToPrint->urgPtr);
	printf("Options:		%u\n\n", segmentToPrint->options);

	//Print to output file

	fprintf(output, "        -Segment Fields-\n");
        fprintf(output, "Source Port:            %hu\n", segmentToPrint->sourcePort);
        fprintf(output, "Destination Port:       %hu\n", segmentToPrint->destPort);
        fprintf(output, "Sequence Number:        %hu\n", segmentToPrint->seqNumber);
        fprintf(output, "Acknowledgement Number: %u\n", segmentToPrint->ackNumber);
        fprintf(output, "Offset:                 %hu\n", segmentToPrint->offset);
        fprintf(output, "URG Flag:               %hu\n", segmentToPrint->URG_flag);
        fprintf(output, "ACK Flag:               %hu\n", segmentToPrint->ACK_flag);
        fprintf(output, "PSH Flag:               %hu\n", segmentToPrint->PSH_flag);
        fprintf(output, "RST Flag:               %hu\n", segmentToPrint->RST_flag);
        fprintf(output, "SYN Flag:               %hu\n", segmentToPrint->SYN_flag);
        fprintf(output, "FIN Flag:               %hu\n", segmentToPrint->FIN_flag);
        fprintf(output, "Receive Window:         %hu\n", segmentToPrint->recvWindow);
        fprintf(output, "Checksum:               %hu\n", segmentToPrint->checksum);
        fprintf(output, "Urgent Ptr:             %hu\n", segmentToPrint->urgPtr);
        fprintf(output, "Options:                %u\n\n", segmentToPrint->options);
}

/* Initializes the bits inside a newly constructed segment (zeros the bits)
*/
void segInit(struct TCP_segment* segmentToInit)
{
	// Setting all relevant values to 0, expect for the offset, which should be the size of
	// our segment in intervals of 32 bits, which will be the same for each segment.
	// Ports and sequence numbers should be adjusted manually.

	segmentToInit->offset	  = 6; //24 bytets, or ((8 x 24) / 32 = 6)
	segmentToInit->reserved	  = 0; //Just setting to zero for consistency
	segmentToInit->URG_flag	  = 0;
	segmentToInit->ACK_flag	  = 0;
	segmentToInit->PSH_flag	  = 0;
	segmentToInit->RST_flag	  = 0;
	segmentToInit->SYN_flag	  = 0;
	segmentToInit->FIN_flag   = 0;
	segmentToInit->recvWindow = 0;
	segmentToInit->checksum	  = 0;
	segmentToInit->urgPtr	  = 0;
	segmentToInit->options	  = 0;
}
