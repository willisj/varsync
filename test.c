
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "varsync.h"
#define EXAMPLE_ID	1
#define USAGE "ERROR:\n\tusage: test [server|client] [send|recv]\n"

int main(int argv, char * argc[]){
	uint8_t role 			 = 0;
	uint32_t example_variable	 = 0;
	struct sync_list * list		 = NULL;
	char  example_buffer[] = "this is some sample data for the buffer";
	struct variable_handler * variable_handle = NULL;
	struct variable_handler * buffer_handle = NULL;
	pthread_t thread_handler;

	if( argv == 3 ){
		if( !strcmp(argc[1], "server"))
			role =  setbit( role, VSROLE_SERVER);
		else if( !strcmp(argc[1], "client"))
			role =  setbit( role, VSROLE_CLIENT);
		else{
			fprintf( stderr, USAGE);
			exit(1);
		}
			
		if( !strcmp(argc[2], "send"))
			role =  setbit( role, VSROLE_SENDER);
		else if( !strcmp(argc[2], "recv"))
			role =  setbit( role, VSROLE_RECEIVER);
		else{
			fprintf( stderr, USAGE);
			exit(1);
		}
	}
	else{
		fprintf( stderr, USAGE);
		exit(1);
	}

	fprintf( stderr, "Initializing list\n");
	list = init_list( role, "127.0.0.1",  5340);
	if(!list){
		fprintf( stderr, "\t\t[Failed]\n");
		exit(1);
	}
	fprintf( stderr, "\t\t[Done]\n");

	
	fprintf( stderr, "Initializing variable\n");
	variable_handle = init_variable_handler( EXAMPLE_ID, &example_variable, sizeof(uint32_t), NULL);
	if(!variable_handle){
		fprintf( stderr, "\t\t[Failed]\n");
		exit(1);
	}
	buffer_handle = init_variable_handler( EXAMPLE_ID + 1, example_buffer, strlen(example_buffer), NULL);
	if(!buffer_handle){
		fprintf( stderr, "\t\t[Failed]\n");
		exit(1);

	}
	fprintf( stderr, "\t\t[Done]\n");

	fprintf( stderr, "Adding variable to list\n");
	if(!add_var_handler( list, variable_handle) ){
		fprintf( stderr, "\t\t[Failed]\n");
		fprintf( stderr, "Unable to add handler to  list\n");
		return 1;
	}
	if(!add_var_handler( list, buffer_handle) ){
		fprintf( stderr, "\t\t[Failed]\n");
		fprintf( stderr, "Unable to add handler to  list\n");
		return 1;
	}
	fprintf( stderr, "\t\t[Done]\n");

	fprintf( stderr, "Creating thread\n");
	if(pthread_create(&thread_handler, NULL, update_thread, list)){
		fprintf( stderr, "\t\t[Failed]\n");
		perror("ERROR creating thread.");
	}

	fprintf( stderr, "\t\t[Done]\n");
	while(1){
		if( getbit( role, VSROLE_SENDER))
			printf("Var: %d\n", example_variable++);
		else
			print_json(   list);
		sleep(1);
	}

	deinit_list(list);

	return 0;
}
