#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "varsync.h"
#include "bsock.h"
/*
#define ROLE_SERVER 	1
#define ROLE_CLIENT 	2 
#define ROLE_SENDER 	4
#define ROLE_RECEIVER	8

struct variable_handler{
	void * var_ptr;
	size_t var_size;
	struct variable_handler * next;
};

struct sync_list{
	struct variable_handler * head;
	uint8_t role;
};

*/

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

struct variable_handler * decode_var(size_t buffer_size, void * in_buffer){
	struct variable_handler * var = malloc(sizeof(struct variable_handler));
	struct variable_handler * in_var = (struct variable_handler *)in_buffer;
	void * newbuff;
	if(!var) return NULL;

	memcpy( &(var->id), &(in_var->id), 4);
	memcpy( &(var->var_size), &(in_var->var_size), 4);

	if( buffer_size != var->var_size + 8 ){
		fprintf( stderr, "Packet size does not match var_size\n");
	       	return NULL;
	}
	
	newbuff = malloc(sizeof(var->var_size));
	if(!newbuff){
		fprintf( stderr, "Failed to allocate buffer for decoded value.\n");
	       	return NULL;
	}

	memcpy( newbuff, in_buffer +8, var->var_size);
	
	var->var_ptr = newbuff;
	return var;
}

size_t encode_var(struct variable_handler * var, void ** out_buffer){
	const size_t calc_size =( sizeof(uint32_t) * 2) + var->var_size;
	uint8_t * buffer = malloc(calc_size);
	
	((struct variable_handler *)buffer)->id = var->id;
	((struct variable_handler *)buffer)->var_size = var->var_size;
	memcpy(buffer + (sizeof(uint32_t) * 2), var->var_ptr, var->var_size);

	*out_buffer = buffer; 
	return calc_size;
}

void* update_thread(void * list_p){
	struct sync_list* list = (struct sync_list *) list_p;
	struct variable_handler* current = list->head;
	uint8_t * packet_buff; 
	size_t txBytes, rxBytes;
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

void deinit_list(struct sync_list* list){
	struct variable_handler* current = list->head;
	struct variable_handler* next;
	while(current){
		next = current->next;
		free(current);
		current = next;
	}
}

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
