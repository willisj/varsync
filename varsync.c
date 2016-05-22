#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "varsync.h"
#include "bsock.h"

/* * * * * * * * * * * * * * * * * * * * * * * ||
 * * * |  UTILITY FUNCTIONS            | * * * ||
 * * * * * * * * * * * * * * * * * * * * * * * || */

/* Prints the buffer out */
int pb(uint8_t *buff, size_t sz){
	size_t i,j;
	for( i = 0; i < sz; ++i){
		printf("%02x ", *(buff + i));

		if(!(i % 15) && i){
			printf("    ");
			for(j = i - 16; j <= i; ++j)
				printf("%c", ( *(buff + j) >= ' ' ? *(buff + j) : '.'  ));

			printf("\n");
		}
		else if(!(i % 7) && i){
			printf("  ");
		}
	}
	
	if(i % 15 && i){
		printf("    ");
		for(j = i - 16; j <= i; ++j)
			printf("%c", ( *(buff + j) >= ' ' ? *(buff + j) : '.'  ));
	}
		
	printf("\n");
}

int  setbit( int input, int bit, int value){
	unsigned int mask = 1;
	mask <<= bit - 1; 
	
	// clear the bit
	input &= ~mask;

	// set the bit if the bool is true
	if(value != 0)
		input |= mask;
	
	return input;
}

int  getbit( int input, int bit, int value){
	unsigned int mask = 1;
	mask <<= bit - 1; 
	
	// clear the other bits
	input &= mask;

	if( ( value == 0  && input == 0 )|| ( value != 0 && input != 0 ) ){
		return 1;
	}
	
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * ||
 * * * |  LIST FUNCTIONS               | * * * ||
 * * * * * * * * * * * * * * * * * * * * * * * || */

/* Initialize the list + wait for connection */
struct sync_list* init_list(uint8_t role, const char * ip, unsigned int port){
	struct sync_list* list = (struct sync_list*) malloc( sizeof( struct sync_list));
	if( !list ){
		fprintf( stderr, "%s:%d: Malloc failed",__func__,__LINE__);
		return NULL;
	}
	
	list->head = NULL;
	list->role = role;

	if(getbit(list->role,VSROLE_SERVER)){
		printf("Waiting for connection\n");
		if( CONNECT_FAIL ==  bsock_listenTCP(port) ){
			free(list);
			return NULL;
		}	
	}
	else {
		if( CONNECT_FAIL == bsock_connectTCP(ip, port)){
			free(list);
			return NULL;
		}
	}	

	return list;
}

/* Add a handler to the list*/
int add_var_handler(struct sync_list * list, struct variable_handler* handle){
	struct variable_handler* current = list->head;
	struct variable_handler* prev;
	if( !current ){
		list->head = handle;
		return 1;
	}
	
	do{
		prev = current;
		current = current->next;
	}while( current );
	
	prev->next = handle;

	return 1;
}

/* decode sent handler from buffer */
struct variable_handler * decode_var(size_t buffer_size, void * in_buffer){
	struct variable_handler * var = malloc(sizeof(struct variable_handler));
	struct variable_handler * in_var = (struct variable_handler *)in_buffer;
	void * newbuff;

	if(!var) return NULL;

	memcpy( &(var->id), &(in_var->id), 4);
	memcpy( &(var->var_size), &(in_var->var_size), 4);

	if( buffer_size != var->var_size + HEADER_METADATA_SIZE ){
		fprintf( stderr, "Packet size does not match var_size\n");
	       	return NULL;
	}
	
	newbuff = malloc(sizeof(var->var_size));
	if(!newbuff){
		fprintf( stderr, "Failed to allocate buffer for decoded value.\n");
	       	return NULL;
	}

	memcpy( newbuff, in_buffer + HEADER_METADATA_SIZE, var->var_size);
	
	var->var_ptr = newbuff;
	return var;
}

/* Encode a variable to be sent onto a buffer */
size_t encode_var(struct variable_handler * var, void ** out_buffer){
	const size_t calc_size =  var->var_size + HEADER_METADATA_SIZE;
	uint8_t * buffer = malloc(calc_size);
	struct variable_handler * out_var = (struct variable_handler *) buffer;
	 
	out_var->id = var->id;
	out_var->var_size = var->var_size;

	memcpy(&(out_var->var_ptr), var->var_ptr, var->var_size);

	*out_buffer = buffer; 
	return calc_size;
}

/*  based on the roles, send or receive updates */
void* update_thread(void * list_p){
	struct sync_list* list = (struct sync_list *) list_p;
	struct variable_handler* current = list->head;
	size_t txBytes, rxBytes;
	uint8_t * packet_buff; 

	txBytes = rxBytes = 0;

	while( 1 ){
		if(getbit(list->role,VSROLE_SENDER)){ 
			size_t sz = encode_var( current, (void **)&packet_buff);

			// advance or reset the list pointer
			if( list->head == NULL ){
				current = NULL;
				sleep(2);
			}
			else if( !current->next )
				current = list->head;
			else
				current = current->next;

			// nothing in the list
			if( current == NULL )
				continue;
			if(!packet_buff || sz == 0){
				fprintf(stderr, "Failed to encode packet\n");
				continue;
			}

			if( bsock_writeTCP( packet_buff, sz) < 0 )
				fprintf(stderr, "Error sending update\n");
			else
				txBytes += sz;
			
			free(packet_buff);
		}
		else{ 		// Receiver
			struct variable_handler * temp_var;
			uint8_t buffer[MAX_PACKETSIZE];
			size_t sz = bsock_readTCP( buffer, MAX_PACKETSIZE );
			
			if( sz < 0 ){
				fprintf(stderr, "Error receiving update\n");
				continue;
			}
			else if(sz == 0)
				continue;
			else
				rxBytes += sz;
		
			temp_var = decode_var( sz, buffer);
			
			if( !temp_var ){
				fprintf( stderr, "Failed to decode packet\n");
				continue;
			}
			
			if( !insert_or_update(list, temp_var) ){
				free(temp_var);
			}
		}
		sleep(1);
	}
}

/* Print the list out to json */
void print_json( struct sync_list* list){
	struct variable_handler* current = list->head;
	
	if( current == NULL) 
		return;

	do {
		printf("{ \"id\" : %u, ", current->id);
		printf(" \"sz\" : %u, ", current->var_size);

		if( current->var_size <= 4){
			printf("\"val\": %d", *((int32_t *) current->var_ptr));

		}
		else{
			int i; 
			printf("\"val\": \"");
			for( i = 0; i < current->var_size; ++i)
				printf( "%02x", *((int8_t *)(current->var_ptr + i)));
			printf("\"");
		}
		
		printf("}\n");
		current = current->next;
	}while( current );
}

/* Search for the var in the list. If it exists, update the value. Otherwise, add it. */
int insert_or_update( struct sync_list* list, struct variable_handler * temp_var){
	struct variable_handler* current = list->head;
	struct variable_handler* prev;
	
	if( current == NULL) {
		list->head = temp_var;
		return 1;
	}

	do {
		if( temp_var->id == current->id ){
			memcpy(current->var_ptr, temp_var->var_ptr, current->var_size);
			return 0;
		}

		prev = current;
		current = current->next;
	}while( current );

	prev->next = temp_var;
		
	return 1;
}

/* Deinit the whole list */ 
void deinit_list(struct sync_list* list){
	struct variable_handler* current = list->head;
	struct variable_handler* next;
	while(current){
		next = current->next;
		free(current);
		current = next;
	}
}

/* Get the tail of the list */
struct variable_handler* get_tail( void * list_p){
	struct sync_list* list = (struct sync_list*)list_p;
	struct variable_handler* current = list->head;
	struct variable_handler* prev;
	if(!current)
		return NULL;
	do{
		prev = current;
		current = current->next;
	}while( current );

	return current;
}

/* * * * * * * * * * * * * * * * * * * * * * * ||
 * * * |  VARHANDLER FUNCTIONS         | * * * ||
 * * * * * * * * * * * * * * * * * * * * * * * || */
struct variable_handler* init_variable_handler(int id, void * var_ptr, size_t var_size, struct variable_handler * next){
	struct variable_handler* handle;
	handle = (struct variable_handler *) malloc( sizeof( struct variable_handler));
	if( !handle ){
		fprintf( stderr, "%s:%d: Malloc failed",__func__,__LINE__);
		return NULL;
	}

	handle->id = id;
	handle->var_ptr = var_ptr;
	handle->var_size = var_size;
	handle->next = NULL;

	return handle;
}
