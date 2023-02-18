#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include "segment.c"

int main()
{
    	int listen_fd, conn_fd;
    	struct sockaddr_in servaddr;
	int servport = 22000, cliport = 33000;

	FILE *fout = fopen("server.out", "w"); // write only

    	/* AF_INET - IPv4 IP , Type of socket, protocol*/
    	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    	bzero(&servaddr, sizeof(servaddr));

    	servaddr.sin_family = AF_INET;
    	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    	servaddr.sin_port = htons(servport); /* Pick another port number to avoid conflict with other students */

    	/* Binds the above details to the socket */
	bind(listen_fd,  (struct sockaddr *) &servaddr, sizeof(servaddr));
	/* Start listening to incoming connections */
	listen(listen_fd, 10);

	//Initialize sending and receiving segments
	struct TCP_segment recvseg;
        segInit(&recvseg);

	struct TCP_segment sendseg;
	segInit(&sendseg);

	//Initialize server TCP
	sendseg.destPort = cliport;
	sendseg.sourcePort = servport;
	sendseg.seqNumber = 85;
	calcChkSum(&sendseg);

    	while(1)
    	{
      		/* Accepts an incoming connection */
	  	conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);

		recv(conn_fd, &recvseg, sizeof(recvseg), 0);

		//Acknowledgment of connection request
		printf("Connection request segment received:\n");
		sendseg.SYN_flag = 1;

		sendseg.ACK_flag = 1;
		sendseg.ackNumber = recvseg.seqNumber + 1;
		calcChkSum(&sendseg);

		printSegment(&sendseg, fout);
		send(conn_fd, (struct TCP_segment*) &sendseg, sizeof(sendseg), 0);

		//Resest bits
		recvseg.SYN_flag = 0;
		sendseg.FIN_flag = 0;

		recv(conn_fd, &recvseg, sizeof(recvseg), 0);
		recv(conn_fd, &recvseg, sizeof(recvseg), 0);

		//Acknowledge close request
		printf("Close request segment received:\n");
		sendseg.SYN_flag = 0;
		sendseg.seqNumber = 63;
		sendseg.ackNumber = recvseg.seqNumber + 1;
		sendseg.ACK_flag = 1;
		calcChkSum(&sendseg);

		printSegment(&sendseg, fout);
		send(conn_fd, (struct TCP_segment*) &sendseg, sizeof(sendseg), 0);

		//Simulate another close acknowledgment
		printf("Grant closing of connection segment:\n");
		sendseg.seqNumber++;
		sendseg.FIN_flag = 1;
		sendseg.ACK_flag = 0;
		calcChkSum(&sendseg);

		printSegment(&sendseg, fout);
		send(conn_fd, (struct TCP_segment*) &sendseg, sizeof(sendseg), 0);

		fclose(fout);	//close file
     		close (conn_fd); //close the connection
    	}
}
