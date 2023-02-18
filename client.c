#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "segment.c"

int main(int argc,char **argv)
{
    	int sockfd, n;
    	int len = sizeof(struct sockaddr);
	int servport = 22000, cliport = 33000;
    	struct sockaddr_in servaddr, cliaddr;

	FILE *fout = fopen("client.out", "w"); //write only

    	/* AF_INET - IPv4 IP , Type of socket, protocol*/
    	sockfd=socket(AF_INET, SOCK_STREAM, 0);
    	bzero(&servaddr,sizeof(servaddr));

    	servaddr.sin_family=AF_INET;
    	servaddr.sin_port=htons(servport); // Server port number

    	/* Convert IPv4 and IPv6 addresses from text to binary form */
	inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));

	//Set client info
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr = INADDR_ANY;
	cliaddr.sin_port = cliport;

	bind(sockfd, (struct sockaddr*) &cliaddr, sizeof(struct sockaddr_in));

    	/* Connect to the server */
    	connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	//Initialize sending and receiving segments
	struct TCP_segment recvseg;
	segInit(&recvseg);

	struct TCP_segment sendseg;
	segInit(&sendseg);

	//Simulate connection request
	sendseg.destPort = servport;
	sendseg.sourcePort = cliport;
	sendseg.SYN_flag = 1;
	sendseg.seqNumber = 37;
	calcChkSum(&sendseg);

	send(sockfd, (struct TCP_segment*) &sendseg, sizeof(sendseg), 0);
	printf("Initial connection request segment:\n");
	printSegment(&sendseg, fout);

    	while(recvseg.FIN_flag != 1)
    	{
		recv(sockfd, &recvseg, sizeof(recvseg), 0);

		//Acknowledgment of connection response
		printf("Connection granted response segment received:\n");
		sendseg.SYN_flag = 0;
		sendseg.ACK_flag = 1;
		sendseg.ackNumber = recvseg.seqNumber + 1;
		sendseg.seqNumber++;
		calcChkSum(&sendseg);

		printSegment(&sendseg, fout);
		send(sockfd, (struct TCP_segment*) &sendseg, sizeof(sendseg), 0);

		sendseg.ACK_flag = 0;
		sendseg.ackNumber = 0;

                //Simulate close request
                printf("Initial close request segment:\n");
                sendseg.seqNumber = 141;
                sendseg.FIN_flag = 1;
                calcChkSum(&sendseg);

                printSegment(&sendseg, fout);
                send(sockfd, (struct TCP_segment*) &sendseg, sizeof(sendseg), 0);

		recv(sockfd, &recvseg, sizeof(recvseg), 0);
		recv(sockfd, &recvseg, sizeof(recvseg), 0);

		//Acknowledgement of close reponse
		printf("Close granted response segment received:\n");
		sendseg.FIN_flag = 0;
		sendseg.ackNumber = recvseg.seqNumber + 1;
		sendseg.seqNumber++;
		sendseg.ACK_flag = 1;
		calcChkSum(&sendseg);

		printSegment(&sendseg, fout);
		send(sockfd, (struct TCP_segment*) &sendseg, sizeof(sendseg), 0);

		fclose(fout);	//close file
		close(sockfd);	//close connecetion
    	}
}
