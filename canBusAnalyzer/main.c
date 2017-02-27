/**
 * @defgroup CANFRMASTER CANFRMASTER
 * @{
 * 		@file main.c
 * 		@brief      main task that's running in CANFR-HW,
 * 		@bug haven't think about how to shut down beagle bone properly 
 * @}
 * @brief      main task that's running in CANFR-HW, 
 * Beaglebone Black which have a duty to commnicate with CANNIBESSA 
 * and interact with other hardware
 * @bug haven't think about how to shut down beagle bone properly
 * 
 */
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <native/timer.h>
#include <printActivation.h>
/* include xenomai lib*/
#include <native/task.h>
#include <rtdk.h>
#include <canfrMsg.h>
#include <canfr_typedef.h>
#include <canfr_canconfig.h>
#include <canfr_canrecv.h>
/*! tcp server port for client connection*/
#define port 1234
/*! buffer size for receving message*/
#define MAX_RX_BUFFER_SIZE 1024
/*! buffer size for transmiting message*/
#define MAX_TX_BUFFER_SIZE 4098

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

/**
 * @brief      communicate with client using tcp protocol CANFR hardware act as server
 * after connected the connection will continue without closing unless everything is done and client send command to disconnect.
 *
 * @return     should not return it it return mean server is not running anymore
 */
int tcpserver()
{
	int server_fd, client_fd, err;
	struct sockaddr_in server, client;
	unsigned char buf[MAX_RX_BUFFER_SIZE];
	//struct hostent *hostp; /* client host info */
	//canfrControlFrameStruct_t currentCtrl;
	//enum responseCodes responseCode;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (server_fd < 0) 
		rt_printf("Could not create socket\n");

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	int opt_val = 1;
	
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

	err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
	
	if (err < 0) 
		rt_printf("Could not bind socket\n");

	err = listen(server_fd, 128);
	
	if (err < 0) 
		rt_printf("Could not listen on socket\n");

	rt_printf("Server is listening on %d\n", port);
	

	socklen_t client_len = sizeof(client);

	while (1)
	{
		client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
		if (client_fd < 0) 
		{
			rt_printf("Could not establish new connection\n");
		}
		
		/* 
	     * read: read input string from the client
	     */
	    bzero(buf, MAX_RX_BUFFER_SIZE);
	    int rNum = read(client_fd, buf, MAX_RX_BUFFER_SIZE);
	    if (rNum < 0) 
	      error("ERROR reading from socket");

	  	rt_printf("printt %d \n", rNum);
	  	rt_printf("printt %s \n", buf);
	    rt_printf("server received %d bytes: %s", rNum, buf);

	     /* 
	     * write: echo the input string back to the client 
	     */
	    rNum = write(client_fd, buf, strlen(buf));
	    if (rNum < 0) 
	      error("ERROR writing to socket");

	    close(client_fd);
	}
	

	  
	return 0;
}

static void init_xenomai() 
{
  /* Avoids memory swapping for this program */
  mlockall(MCL_CURRENT|MCL_FUTURE);

  /* Perform auto-init of rt_print buffers if the task doesn't do so */
  rt_print_auto_init(1);
}

/**
 * @brief      strat up function
 *
 * @return     no meaning
 */
int main(void)
{
	RT_TASK task1;
	init_xenomai();
	
	/*change current task to be a realtime task*/
	rt_task_shadow(&task1, "Task 1", 10, 0);
	rt_timer_set_mode(0);
	canfr_canSetBit(ITF_RTCAN0,500000);
	canfr_canSetBit(ITF_RTCAN1,125000);
	canfr_canActivate(ITF_RTCANALL,0);
	canfr_canrecvResetClock();
	canfr_canrecvStart(ITF_RTCANALL);
	tcpserver();

	return 0;
}
